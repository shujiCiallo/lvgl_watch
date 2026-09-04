#include <stdint.h>
#include "lvgl.h"
#include "music.h"
#include "style/app_colors.h"
#include "style/app_fonts.h"

#define MUSIC_LIST_ROW_H  44

static lv_obj_t *music_label_create(lv_obj_t *parent);
static lv_obj_t *music_ctl_create(lv_obj_t *parent);
static lv_obj_t *music_tools_create(music_panel_t *self, lv_obj_t *parent);
static lv_obj_t *music_list_create(music_panel_t *self);
static lv_obj_t *music_volume_create(music_panel_t *self);
static void music_view_show(music_panel_t *self, lv_obj_t *view);
static void btn_create(lv_obj_t *parent);
static void arc_progress_obs_cb(lv_observer_t *obs, lv_subject_t *sub);
static void time_text_obs_cb(lv_observer_t *obs, lv_subject_t *sub);
static void mode_text_obs_cb(lv_observer_t *obs, lv_subject_t *sub);
static void mode_click_cb(lv_event_t *e);
static void list_highlight_obs_cb(lv_observer_t *obs, lv_subject_t *sub);
static void row_click_cb(lv_event_t *e);
static void to_list_click_cb(lv_event_t *e);
static void to_volume_click_cb(lv_event_t *e);
static void to_player_click_cb(lv_event_t *e);
static void swipe_back_cb(lv_event_t *e);
static void volume_changed_cb(lv_event_t *e);
static void volume_slider_obs_cb(lv_observer_t *obs, lv_subject_t *sub);
static void volume_text_obs_cb(lv_observer_t *obs, lv_subject_t *sub);

/* 动态中文字体,CJK 字库缺失时回退内置 latin(曲名可能显示不全) */
static const lv_font_t *cjk_font(int32_t size, const lv_font_t *fallback)
{
    const lv_font_t *font = app_font_cjk(size);
    return font != NULL ? font : fallback;
}

void music_panel_create(music_panel_t *self)
{
    self->root = lv_obj_create(NULL);
    lv_obj_set_size(self->root, lv_pct(100), lv_pct(100));

    /* 两个全屏兄弟子视图:歌单 / 播放页,靠 HIDDEN 互斥切换 */
    self->view_list = lv_obj_create(self->root);
    lv_obj_set_size(self->view_list, lv_pct(100), lv_pct(100));

    self->view_player = lv_obj_create(self->root);
    lv_obj_set_size(self->view_player, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(self->view_player, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(self->view_player, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_scrollable(self->view_player, false);   /* 播放页固定,防拖走 */

    self->label = music_label_create(self->view_player);
    self->btn = music_ctl_create(self->view_player);
    self->tools = music_tools_create(self, self->view_player);
    music_list_create(self);
    music_volume_create(self);

    music_view_show(self, self->view_player);
}

static void music_view_show(music_panel_t *self, lv_obj_t *view)
{
    lv_obj_set_hidden(self->view_list, view != self->view_list);
    lv_obj_set_hidden(self->view_player, view != self->view_player);
    lv_obj_set_hidden(self->view_volume, view != self->view_volume);
}

/* navigator_push 之后调用:二级页右滑收回页内(list/volume → 播放页),
 * 复位 gesture_bubble 使手势落在二级页自身而非冒泡到 root 触发导航 pop */
void music_panel_gesture_install(music_panel_t *self)
{
    lv_obj_set_gesture_bubble(self->view_list, false);
    lv_obj_add_event_cb(self->view_list, swipe_back_cb, LV_EVENT_GESTURE, self);

    lv_obj_set_gesture_bubble(self->view_volume, false);
    lv_obj_add_event_cb(self->view_volume, swipe_back_cb, LV_EVENT_GESTURE, self);
}

static void swipe_back_cb(lv_event_t *e)
{
    music_panel_t *self = lv_event_get_user_data(e);
    if (lv_indev_get_gesture_dir(lv_indev_active()) == LV_DIR_RIGHT) {
        music_view_show(self, self->view_player);
    }
    lv_event_stop_bubbling(e);   /* 二级页手势一律不上冒,避免误 pop 整页 */
}

/* ---- 子视图:歌单列表 ---- */

static lv_obj_t *music_list_create(music_panel_t *self)
{
    lv_obj_t *list = self->view_list;
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(list, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START);

    /* 头部 */
    lv_obj_t *header = lv_label_create(list);
    lv_obj_set_width(header, lv_pct(100));
    lv_obj_set_style_text_align(header, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(header, cjk_font(20, &lv_font_montserrat_20), 0);
    lv_obj_set_style_text_color(header, COLOR_PRIMARY, 0);
    lv_obj_set_style_pad_top(header, 10, 0);
    lv_obj_set_style_pad_bottom(header, 10, 0);
    lv_label_set_text(header, "歌曲列表");

    /* 行容器:可纵向滚动 */
    lv_obj_t *rows = lv_obj_create(list);
    lv_obj_set_width(rows, lv_pct(100));
    lv_obj_set_flex_grow(rows, 1);
    lv_obj_set_scroll_dir(rows, LV_DIR_VER);   /* 只竖滚:横滑成为右滑手势 */
    lv_obj_set_style_pad_all(rows, 0, 0);
    lv_obj_set_style_border_width(rows, 0, 0);
    lv_obj_set_flex_flow(rows, LV_FLEX_FLOW_COLUMN);
    self->list_rows = rows;

    uint16_t n = music_core_song_count();
    for (uint16_t i = 0; i < n; i++) {
        const music_song_t *song = music_core_song(i);
        lv_obj_t *row = lv_obj_create(rows);
        lv_obj_remove_style_all(row);
        lv_obj_set_size(row, lv_pct(100), MUSIC_LIST_ROW_H);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_START);
        lv_obj_set_style_pad_left(row, 16, 0);
        lv_obj_set_style_pad_right(row, 16, 0);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_clickable(row, true);
        lv_obj_set_user_data(row, (void *)(uintptr_t)i);
        lv_obj_add_event_cb(row, row_click_cb, LV_EVENT_CLICKED, self);

        lv_obj_t *nm = lv_label_create(row);
        lv_obj_set_width(nm, lv_pct(100));
        lv_obj_set_scrollable(nm, false);
        lv_label_set_long_mode(nm, LV_LABEL_LONG_DOT);
        lv_obj_set_style_text_font(nm, cjk_font(20, &lv_font_montserrat_20), 0);
        lv_obj_set_style_text_color(nm, COLOR_TEXT_PRIMARY, 0);
        lv_label_set_text(nm, song->name);
    }

    if (n == 0) {
        lv_obj_t *empty = lv_label_create(rows);
        lv_obj_set_style_pad_top(empty, 24, 0);
        lv_obj_set_style_text_align(empty, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(empty, cjk_font(20, &lv_font_montserrat_20), 0);
        lv_obj_set_style_text_color(empty, COLOR_TEXT_SECONDARY, 0);
        lv_label_set_text(empty, "暂无音频文件");
    }

    /* 当前播放行高亮(立即按当前 index 刷新一次) */
    lv_subject_add_observer_obj(&g_music_index_subject, list_highlight_obs_cb,
        rows, NULL);
    return list;
}

/* 歌单行选中态观察者:遍历行容器,命中 index 的行反色 */
static void list_highlight_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *rows = lv_observer_get_target(obs);
    int32_t cur = lv_subject_get_int(sub);
    uint32_t cnt = music_core_song_count();
    uint32_t n = lv_obj_get_child_count(rows);
    for (uint32_t i = 0; i < n; i++) {
        lv_obj_t *row = lv_obj_get_child(rows, i);
        uintptr_t idx = (uintptr_t)lv_obj_get_user_data(row);
        int sel = (idx < cnt) && ((int32_t)idx == cur);
        lv_obj_set_style_bg_color(row, COLOR_PRIMARY, 0);
        lv_obj_set_style_bg_opa(row, sel ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
        lv_obj_t *nm = lv_obj_get_child(row, 0);
        if (nm != NULL) {
            lv_obj_set_style_text_color(nm,
                sel ? COLOR_SURFACE : COLOR_TEXT_PRIMARY, 0);
        }
    }
}

static void row_click_cb(lv_event_t *e)
{
    lv_obj_t *row = lv_event_get_current_target(e);
    music_panel_t *self = lv_event_get_user_data(e);
    int idx = (int)(uintptr_t)lv_obj_get_user_data(row);
    if ((uintptr_t)idx >= music_core_song_count()) return;
    music_core_play_index(idx);
    music_view_show(self, self->view_player);
}

/* ---- 播放页:顶部曲名区 ---- */

static lv_obj_t *music_label_create(lv_obj_t *parent)
{
    lv_obj_t *label = lv_obj_create(parent);
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_set_flex_grow(label, 4);
    lv_obj_set_scrollable(label, false);
    lv_obj_set_flex_flow(label, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(label, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t *label_title = lv_label_create(label);
    lv_obj_set_width(label_title, lv_pct(100));
    lv_obj_set_scrollable(label_title, false);
    lv_obj_set_flex_grow(label_title, 5);
    lv_obj_set_style_text_font(label_title, cjk_font(24, &lv_font_montserrat_20), 0);
    lv_obj_set_style_text_align(label_title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label_title, COLOR_PRIMARY, 0);
    lv_label_set_long_mode(label_title, LV_LABEL_LONG_DOT);
    lv_label_bind_text(label_title, &g_music_title_subject, NULL);

    lv_obj_t *label_time = lv_label_create(label);
    lv_obj_set_width(label_time, lv_pct(100));
    lv_obj_set_scrollable(label_time, false);
    lv_obj_set_flex_grow(label_time, 3);
    lv_obj_set_style_text_font(label_time, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_align(label_time, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label_time, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_opa(label_time, LV_OPA_50, 0);
    lv_subject_add_observer_obj(&g_music_pos_subject, time_text_obs_cb,
        label_time, NULL);
    lv_subject_add_observer_obj(&g_music_duration_subject, time_text_obs_cb,
        label_time, NULL);

    return label;
}

/* ---- 播放页:控制区 ---- */

static lv_obj_t *music_ctl_create(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_flex_grow(obj, 6);
    lv_obj_set_scrollable(obj, false);   /* 控制区固定,禁止拖动画面 */

    /* 进度环:整圈轨道(0~360 全环),轨道半透明始终可见(value=0 也有轮廓);
     * indicator 从右侧(3点)起顺时针随已播秒数填充。注:未用 rotation 偏移起点,
     * 因 arc 内部按 360 归一化失效区域会出错(此前 270/630 归成 270/270 致全空)。 */
    lv_obj_t *arc = lv_arc_create(obj);
    lv_obj_set_size(arc, 120, 120);
    lv_obj_center(arc);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_range(arc, 0, 1);
    lv_arc_set_value(arc, 0);
    lv_obj_set_clickable(arc, false);
    lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_border_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_arc_width(arc, 12, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(arc, LV_OPA_50, LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x9E9E9E), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, 12, LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(arc, LV_OPA_50, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_color_hex(0x000000), LV_PART_INDICATOR);
    lv_subject_add_observer_obj(&g_music_pos_subject, arc_progress_obs_cb,
        arc, NULL);
    lv_subject_add_observer_obj(&g_music_duration_subject, arc_progress_obs_cb,
        arc, NULL);

    btn_create(obj);

    return obj;
}

static void prev_click_cb(lv_event_t *e);
static void toggle_click_cb(lv_event_t *e);
static void next_click_cb(lv_event_t *e);
static void toggle_icon_obs_cb(lv_observer_t *obs, lv_subject_t *sub);

static void btn_create(lv_obj_t *parent)
{
    static lv_style_t music_ctl_style;

    lv_style_init(&music_ctl_style);
    lv_style_set_text_font(&music_ctl_style, &lv_font_montserrat_30);
    lv_style_set_text_align(&music_ctl_style, LV_TEXT_ALIGN_CENTER);
    lv_style_set_bg_opa(&music_ctl_style, LV_OPA_TRANSP);

    lv_obj_t *btn = lv_obj_create(parent);
    lv_obj_remove_style_all(btn);
    lv_obj_set_width(btn, lv_pct(100));
    lv_obj_set_height(btn, LV_SIZE_CONTENT);
    lv_obj_center(btn);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_set_scrollable(btn, false);

    lv_obj_t *prev = lv_label_create(btn);
    lv_obj_add_style(prev, &music_ctl_style, 0);
    lv_obj_set_flex_grow(prev, 3);
    lv_label_set_text(prev, LV_SYMBOL_PREV);
    lv_obj_set_size(prev, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_clickable(prev, true);
    lv_obj_add_event_cb(prev, prev_click_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t *toggle = lv_label_create(btn);
    lv_obj_add_style(toggle, &music_ctl_style, 0);
    lv_obj_set_flex_grow(toggle, 5);
    lv_obj_set_style_text_font(toggle, &lv_font_montserrat_40, 0);
    lv_label_set_text(toggle, LV_SYMBOL_PLAY);
    lv_obj_set_size(toggle, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_clickable(toggle, true);
    lv_obj_add_event_cb(toggle, toggle_click_cb, LV_EVENT_CLICKED, NULL);
    lv_subject_add_observer_obj(&g_music_state_subject, toggle_icon_obs_cb,
        toggle, NULL);

    lv_obj_t *next = lv_label_create(btn);
    lv_obj_add_style(next, &music_ctl_style, 0);
    lv_obj_set_flex_grow(next, 3);
    lv_label_set_text(next, LV_SYMBOL_NEXT);
    lv_obj_set_size(next, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_clickable(next, true);
    lv_obj_add_event_cb(next, next_click_cb, LV_EVENT_CLICKED, NULL);
}

/* ---- 播放页:底部工具行 ---- */

static lv_obj_t *music_tools_create(music_panel_t *self, lv_obj_t *parent)
{
    lv_obj_t *tools = lv_obj_create(parent);
    lv_obj_set_width(tools, lv_pct(100));
    lv_obj_set_flex_grow(tools, 4);
    lv_obj_set_flex_flow(tools, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(tools, LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *back = lv_label_create(tools);
    lv_obj_set_style_text_font(back, cjk_font(20, &lv_font_montserrat_20), 0);
    lv_obj_set_style_text_align(back, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(back, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_clickable(back, true);
    lv_obj_add_event_cb(back, to_list_click_cb, LV_EVENT_CLICKED, self);
    lv_label_set_text(back, "列表");

    lv_obj_t *volume = lv_label_create(tools);
    lv_obj_set_style_text_font(volume, cjk_font(20, &lv_font_montserrat_20), 0);
    lv_obj_set_style_text_align(volume, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(volume, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_clickable(volume, true);
    lv_obj_add_event_cb(volume, to_volume_click_cb, LV_EVENT_CLICKED, self);
    lv_label_set_text(volume, "音量");

    lv_obj_t *mode = lv_label_create(tools);
    lv_obj_set_style_text_font(mode, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(mode, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(mode, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_clickable(mode, true);
    lv_obj_add_event_cb(mode, mode_click_cb, LV_EVENT_CLICKED, NULL);
    lv_subject_add_observer_obj(&g_music_mode_subject, mode_text_obs_cb,
        mode, NULL);

    return tools;
}

static void to_list_click_cb(lv_event_t *e)
{
    music_panel_t *self = lv_event_get_user_data(e);
    music_view_show(self, self->view_list);
}

static void to_volume_click_cb(lv_event_t *e)
{
    music_panel_t *self = lv_event_get_user_data(e);
    music_view_show(self, self->view_volume);
}

static void to_player_click_cb(lv_event_t *e)
{
    music_panel_t *self = lv_event_get_user_data(e);
    music_view_show(self, self->view_player);
}

/* ---- 子视图:音量调节 ---- */

static lv_obj_t *music_volume_create(music_panel_t *self)
{
    lv_obj_t *vol = lv_obj_create(self->root);
    lv_obj_set_size(vol, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(vol, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(vol, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(vol, 18, 0);
    self->view_volume = vol;

    lv_obj_t *title = lv_label_create(vol);
    lv_obj_set_style_text_font(title, cjk_font(24, &lv_font_montserrat_24), 0);
    lv_obj_set_style_text_color(title, COLOR_PRIMARY, 0);
    lv_label_set_text(title, "音量");

    lv_obj_t *val = lv_label_create(vol);
    lv_obj_set_style_text_font(val, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(val, COLOR_TEXT_PRIMARY, 0);
    lv_subject_add_observer_obj(&g_music_volume_subject, volume_text_obs_cb,
        val, NULL);

    lv_obj_t *slider = lv_slider_create(vol);
    lv_obj_set_size(slider, 40, 180);   /* 高>宽 → 竖条,上下拖调音量 */
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(slider, lv_color_lighten(COLOR_PRIMARY, LV_OPA_60),
        LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, COLOR_PRIMARY, LV_PART_INDICATOR);
    /* 去掉圆形滑块(仅隐藏 KNOB):按下/滑轨道即可调值 */
    lv_obj_set_style_bg_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_border_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_add_event_cb(slider, volume_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_subject_add_observer_obj(&g_music_volume_subject, volume_slider_obs_cb,
        slider, NULL);

    lv_obj_t *back = lv_label_create(vol);
    lv_obj_set_style_text_font(back, cjk_font(20, &lv_font_montserrat_20), 0);
    lv_obj_set_style_text_color(back, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_clickable(back, true);
    lv_obj_add_event_cb(back, to_player_click_cb, LV_EVENT_CLICKED, self);
    lv_label_set_text(back, "返回");

    return vol;
}

static void volume_changed_cb(lv_event_t *e)
{
    lv_obj_t *slider = lv_event_get_current_target(e);
    music_core_set_volume((int)lv_slider_get_value(slider));
}

static void volume_slider_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *slider = lv_observer_get_target(obs);
    lv_slider_set_value(slider, lv_subject_get_int(sub), LV_ANIM_OFF);
}

static void volume_text_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    (void)obs;
    lv_label_set_text_fmt(lv_observer_get_target(obs), "%d%%",
        lv_subject_get_int(sub));
}

/* ---- subject 观察者与命令 ---- */

/* 播放状态观察者:驱动播放/暂停按钮图标 */
static void toggle_icon_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *label = lv_observer_get_target(obs);
    lv_label_set_text(label, lv_subject_get_int(sub) == MUSIC_STATE_PLAYING ?
        LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
}

static void prev_click_cb(lv_event_t *e)
{
    (void)e;
    music_core_prev();
}

static void toggle_click_cb(lv_event_t *e)
{
    (void)e;
    music_core_toggle();
}

static void next_click_cb(lv_event_t *e)
{
    (void)e;
    music_core_next();
}

/* 进度弧观察者:重设 range/value 保持与当前曲时长一致 */
static void arc_progress_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    (void)sub;
    lv_obj_t *arc = lv_observer_get_target(obs);
    int32_t dur = lv_subject_get_int(&g_music_duration_subject);
    int32_t pos = lv_subject_get_int(&g_music_pos_subject);
    lv_arc_set_range(arc, 0, dur > 0 ? dur : 1);
    lv_arc_set_value(arc, pos);
}

/* 时间文本观察者:"mm:ss / mm:ss"(已播 / 总长),由 pos/duration 驱动 */
static void time_text_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    (void)sub;
    lv_obj_t *label = lv_observer_get_target(obs);
    int32_t pos = lv_subject_get_int(&g_music_pos_subject);
    int32_t dur = lv_subject_get_int(&g_music_duration_subject);
    lv_label_set_text_fmt(label, "%02d:%02d / %02d:%02d",
        (int)pos / 60, (int)pos % 60, (int)dur / 60, (int)dur % 60);
}

/* 播放模式观察者:模式标签文本 */
static void mode_text_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    static const char *const mode_str[] = {"SEQ", "LOOP", "ONE", "SHUF"};
    int32_t mode = lv_subject_get_int(sub);
    lv_obj_t *label = lv_observer_get_target(obs);
    if (mode < 0 || mode > MUSIC_MODE_SHUFFLE) mode = MUSIC_MODE_SEQUENTIAL;
    lv_label_set_text(label, mode_str[mode]);
}

static void mode_click_cb(lv_event_t *e)
{
    (void)e;
    music_core_cycle_mode();
}
