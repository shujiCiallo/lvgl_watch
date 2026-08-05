#include "screen_main.h"
#include "lvgl/lvgl.h"
#include "event/event.h"
#include "style/app_styles.h"
#include "screen/screen_manager.h"
#include "core/data_center.h"

/* 各面板构建函数,只在模块内部使用 */
static void time_panel_create(lv_obj_t *parent);
static void music_panel_create(lv_obj_t *parent);
static void info_panel_create(lv_obj_t *parent);

/* 创建主屏幕(表盘):背景样式 + 时间/音乐/信息 三个面板 */
void screen_main_create(screen_main_t *self, lv_obj_t *parent)
{
    self->root = parent;   /* 保存根对象到结构体,替代全局静态变量 */
    lv_obj_add_style(self->root, &style_screen_bg, 0);
    lv_obj_set_flex_flow(self->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(self->root, lv_pct(100), lv_pct(100));

    time_panel_create(self->root);
    music_panel_create(self->root);
    info_panel_create(self->root);
}

/* 时间面板:大字号时间文本,绑定 subject 自动刷新 */
static void time_panel_create(lv_obj_t *parent)
{
    lv_obj_t *time_panel = lv_obj_create(parent);
    lv_obj_set_align(time_panel, LV_ALIGN_TOP_MID);
    lv_obj_set_size(time_panel, lv_pct(100), lv_pct(40));
    {
        lv_obj_t *time_label = lv_label_create(time_panel);
        lv_obj_center(time_label);
        lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
        /* 时间文本由数据层 g_time_subject 驱动,无需手动刷新 */
        lv_label_bind_text(time_label, &g_time_subject, "%s");
    }
}

/* 音乐面板:上一首/播放/下一首 三个按钮 */
static void music_panel_create(lv_obj_t *parent)
{
    static lv_style_t music_style;   /* 音乐按钮文本样式,仅本函数使用 */

    lv_obj_t *music_panel = lv_obj_create(parent);
    lv_obj_set_size(music_panel, lv_pct(100), lv_pct(35));
    lv_obj_set_flex_flow(music_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(music_panel,
                      LV_FLEX_ALIGN_START,
                      LV_FLEX_ALIGN_CENTER,
                      LV_FLEX_ALIGN_START);

    lv_style_init(&music_style);
    lv_style_set_text_font(&music_style, &lv_font_montserrat_36);

    /* 用 Button_t 配置数组批量创建按钮 */
    Button_t music_button[] = {
        {NULL, NULL, LV_SYMBOL_PREV},
        {NULL, NULL, LV_SYMBOL_PLAY},
        {NULL, NULL, LV_SYMBOL_NEXT}
    };
    size_t num = sizeof(music_button) / sizeof(music_button[0]);
    for (size_t i = 0; i < num; i++){
        music_button[i].btn = lv_btn_create(music_panel);
        music_button[i].label = lv_label_create(music_button[i].btn);
        lv_obj_set_height(music_button[i].btn, lv_pct(100));
        lv_obj_add_style(music_button[i].btn, &music_style, 0);
        lv_obj_set_flex_grow(music_button[i].btn, 1);
        lv_label_set_text(music_button[i].label, music_button[i].text);
        lv_obj_center(music_button[i].label);
    }
}

/* 信息面板:SD卡/定位/电量 三个状态按钮 */
static void info_panel_create(lv_obj_t *parent)
{
    lv_obj_t *info_panel = lv_obj_create(parent);
    lv_obj_set_size(info_panel, lv_pct(100), lv_pct(25));
    lv_obj_set_flex_flow(info_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(info_panel,
                      LV_FLEX_ALIGN_START,
                      LV_FLEX_ALIGN_CENTER,
                      LV_FLEX_ALIGN_START);
    {
        lv_obj_t *temp = lv_btn_create(info_panel);
        lv_obj_set_flex_grow(temp, 1);
        lv_obj_set_height(temp, lv_pct(100));
        lv_obj_t *sim_label = lv_label_create(temp);
        lv_label_set_text(sim_label, LV_SYMBOL_SD_CARD);
        lv_obj_add_style(sim_label, &info_style, 0);
        lv_obj_center(sim_label);

        lv_obj_t *compass = lv_btn_create(info_panel);
        lv_obj_set_flex_grow(compass, 1);
        lv_obj_set_height(compass, lv_pct(100));
        lv_obj_t *compass_label = lv_label_create(compass);
        lv_label_set_text(compass_label, LV_SYMBOL_GPS);
        lv_obj_add_style(compass_label, &info_style, 0);
        lv_obj_center(compass_label);

        lv_obj_t *BAT = lv_btn_create(info_panel);
        lv_obj_set_flex_grow(BAT, 1);
        lv_obj_set_height(BAT, lv_pct(100));
        lv_obj_t *BAT_label = lv_label_create(BAT);
        lv_label_set_text(BAT_label, LV_SYMBOL_BATTERY_1);
        lv_obj_add_style(BAT_label, &info_style, 0);
        lv_obj_center(BAT_label);
        lv_obj_set_scrollbar_mode(BAT, LV_SCROLLBAR_MODE_OFF);
    }
}
