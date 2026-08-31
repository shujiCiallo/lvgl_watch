# LVGL Watch UI 工程代码规范

本文件定义本工程的代码规范，是重构收敛后的约定基线。
- 标注 **【现状】**：当前已落地、新代码必须遵守。
- 标注 **【目标】**：新代码必须遵守，但现有代码尚未完全迁移（见第 10 节遗留清单）。

核心三条纪律：
1. **导航只有一个出口** —— push/pop，返回即销毁。
2. **组件不持有文件级全局** —— 状态进 struct，谁创建谁持有。
3. **UI 不碰业务** —— subject 是唯一数据通道，阈值/规则在 data_center。

---

## 1. 架构分层

```
src/
  core/          数据层:data_center(subject + 业务常量 + 分级规则)
  hal/           硬件抽象(SDL)
  ui/
    screen/      主屏(表盘/工具/应用/卡片),统一 create(self, parent) 签名
    pages/       【目标】全屏详情页(sport/compass/setting),进入创建返回销毁
    event/       【现状·过渡】详情页当前所在目录,逐步迁往 pages/
    widgets/     可复用组件,状态入 struct,无文件级 static
    navigation/  导航控制器(ScreenNavigator)
    style/       全局样式与颜色
```

- 主骨架是 tileview（表盘/工具/卡片/消息/应用 5 面板滑动切换），由 `screen_manager` 创建。
- 全屏详情页是独立模态页，覆盖在主骨架之上，由导航控制器管理，**不挂在 tileview 里**。

## 2. 导航与页面生命周期

- 详情页一律走 ScreenNavigator：
  - 进入：点击回调里调用页面模块的 `xxx_create(&self)` → `navigator_push(self.root)`。
  - 返回：右滑手势由 navigator 统一注册，`navigator_pop()` 销毁页面对象并恢复上层。
- 详情页模块**自建 root**：`xxx_panel_t` 持有 `root`（如 `sport_panel_t`/`compass_panel_t`），`xxx_create(self)` 内部 `lv_obj_create(NULL)` 并配置尺寸/布局/滚动，组件状态也收进 struct；调用方不组装内容。
- **禁止**：
  - 手动 `lv_scr_load()` 切详情页（绕过导航）。
  - 缓存/复用页面对象。详情页零状态（数据全部由 subject 驱动），进入创建、返回销毁，避免对象只增不减的内存泄漏。
  - 页面内自写手势返回回调（由 navigator 统一处理）。

## 3. 组件模式（widgets）

- 状态写入**显式 struct**，由调用者持有。create 签名统一：
  ```c
  void xxx_widget_create(xxx_widget_t *self, lv_obj_t *parent, const xxx_cfg_t *cfg);
  ```
  例：`compass_widget`、`activity_rings`、`stat_label`。
- **【强制】** 禁止用文件级 `static` 存放组件实例状态（多实例会串数据）；`static` 只留给编译期常量和模块私有工具函数。
- 数据通过 cfg 注入 `lv_subject_t *`（依赖注入），组件**不直接引用** `g_*_subject`，保证可复用、可单测。
- observer 一律用 `lv_subject_add_observer_obj`（绑定对象生命周期，对象删除自动移除 observer，避免悬空）。

## 4. 数据层纪律

- UI 只**消费** subject（`lv_label_bind_text` / `lv_subject_add_observer_obj`），不驱动数据采集。
- **【目标】** 采样/统计由 data_center 定时器驱动。UI 创建函数里不调用采样接口（现状：HRmonitor 在 UI 创建时调 `data_center_hr_sample()`，应迁移，见遗留）。
- 业务常量**唯一出口**在 `data_center.h`：
  - 满量程：`CALORIE_MAX` / `STEPS_MAX` / `DURATION_MAX`
  - 电量分档：`BAT_LEVEL_LOW_MAX` / `BAT_LEVEL_MID_MAX` / `BAT_LEVEL_FULL`
  - 分级规则：`bat_symbol_level(uint8_t bat)`
- 同一数值不得在 UI 侧硬编码（如电量三档 34/64/100 只允许出现在 data_center）。

## 5. 命名规范

- include guard：`模块名_H`（如 `SCREEN_MAIN_H`）。禁止 `__` 双下划线开头（编译器保留），禁止过泛名（如 `CARD_H`）。
- 类型 typedef：`xxx_t`。
- 跨模块全局变量：`g_` 前缀；仅模块内使用的必须是 `static`。
- 函数/变量：`snake_case`。
- 回调命名：观察者 `xxx_obs_cb`，点击 `xxx_click_cb`。
- 不写缩水拼写（如 `palymode_t` 应为 `play_mode_t`，见遗留）。

## 6. 头文件与 include

- include 根目录为 `src` 与 `src/ui`，路径风格：
  `"core/data_center.h"`、`"widgets/compass_widget.h"`、`"screen/screen_card.h"`、`"navigation/screen_navigator.h"`。
- 头文件只声明接口，不暴露内部实现；用前置声明减少互相包含。
- 模块状态封装进 struct，经 create 传入；不用裸全局跨文件传递。

## 7. LVGL API 约定

- **【强制】** 用 `lv_obj_set_*` setter（`lv_obj_set_hidden/scrollable/clickable/event_bubble/checkable/click_focusable` 等），禁用已弃用的 `lv_obj_add_flag / lv_obj_remove_flag / lv_obj_clear_flag`。
- 文本绑定用 `lv_label_bind_text`，不手动 `lv_label_set_text` 刷新。
- 事件冒泡统一用 `enable_bubble_all`。
- 引入不熟悉的 API 前先查 LVGL 头文件确认存在（本项目 LVGL 9.6，setter 均已提供）。

## 8. 样式管理

- **【目标】** 通用样式收进 `style/app_styles(.h/.c)`，提供统一入口；新组件优先复用全局样式，不新造局部样式。
- 局部模块级 `static lv_style_t` 仅限一次性特例，命名 `模块名_style`，且只在 create 处初始化一次（重复 init 会浪费并覆盖）。
- 现状的模块级局部样式（`screen_tools` 的 `title_style`、`screen_app` 的 `applist_style`、`setting_page` 的 `title_style`）待迁入 style/，见遗留。

## 9. 构建与提交

- 源码收集用 CMake `file(GLOB_RECURSE)`；**新增 .c 文件后必须重新 `cmake -B build`** 才会被收录。
- git 操作走 `wsl -e bash -c '...'`（规避 Windows 侧 dubious ownership）。
- commit message 用单行，聚焦 why。
- 每个阶段改完：编译无错误/警告 + 运行 `./bin/main` 验证无断言崩溃。

## 10. 已知遗留（待迁移）

| # | 项 | 位置 | 说明 |
|---|---|---|---|
| 1 | 详情页目录迁移 | `ui/event/` → `ui/pages/` | 新详情页放 pages/，现有 event/ 逐步迁移 |
| 2 | 局部样式收进 style/ | screen_tools / screen_app / setting_page | 模块级 static 样式迁到 style/ 统一入口 |
| 3 | UI 不驱动采样 | HRmonitor.c | `data_center_hr_sample()` 应在 UI 创建时调用 → 移到 data_center 定时驱动 |
| 4 | 组件状态入 struct | HRmonitor.c | `low_values/high_values/value_index` 文件级 static，多实例会串数据 |
| 5 | 死代码 music.c | ui/event/music.c | 未接入导航；文件系统扫描应移入 core |
