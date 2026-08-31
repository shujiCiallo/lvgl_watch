#include "screen_main.h"
#include "lvgl/lvgl.h"
#include "event/event.h"
#include "style/app_styles.h"
#include "style/app_colors.h"
#include "screen/screen_manager.h"
#include "core/data_center.h"
#include "widgets/compass_widget.h"
#include "ui_utils.h"


/* 各面板构建函数,只在模块内部使用 */
static void time_panel_create(lv_obj_t *parent);
static void BAT_arc_create(lv_obj_t *parent);
static void calorie_panel_create(lv_obj_t *parent);
static void info_panel_create(lv_obj_t *parent);

/* 卡路里 slider 数据观察者回调 */
static void calorie_slider_obs_cb(lv_observer_t *obs, lv_subject_t *sub);

/* 电量图标观察者回调 */
static void bat_icon_obs_cb(lv_observer_t *obs, lv_subject_t *sub);

/* 创建主屏幕(表盘):背景样式 + 时间/音乐/信息 三个面板 */
void screen_main_create(screen_main_t *self, lv_obj_t *parent)
{
    self->root = parent;   /* 保存根对象到结构体,替代全局静态变量 */
    // lv_obj_remove_style_all(self->root);
    lv_obj_add_style(self->root, &style_screen_bg, 0);
    lv_obj_set_flex_flow(self->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(self->root, lv_pct(100), lv_pct(100));

    time_panel_create(self->root);
    calorie_panel_create(self->root);
    info_panel_create(self->root);
}

/* 时间面板:大字号时间文本,绑定 subject 自动刷新 */
static void time_panel_create(lv_obj_t *parent)
{
    lv_obj_t *time_panel = lv_obj_create(parent);
    lv_obj_set_align(time_panel, LV_ALIGN_TOP_MID);
    lv_obj_set_flex_grow(time_panel, 4);
    // lv_obj_set_size(time_panel, lv_pct(100), lv_pct(40));
    lv_obj_set_width(time_panel, lv_pct(100));
    {
        lv_obj_t *time_label = lv_label_create(time_panel);
        lv_obj_center(time_label);
        lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
        /* 时间文本由数据层 g_time_subject 驱动,无需手动刷新 */
        lv_label_bind_text(time_label, &g_time_subject, "%s");
    }
}

static void calorie_panel_create(lv_obj_t *parent)
{
    lv_obj_t *calorie_panel = lv_obj_create(parent);
    lv_obj_set_flex_flow(calorie_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_grow(calorie_panel, 3);
    lv_obj_set_width(calorie_panel, lv_pct(100));
    lv_obj_set_style_bg_color(calorie_panel, COLOR_SURFACE, 0);
    lv_obj_set_style_pad_all(calorie_panel, 8, 0);
    /* 点击进入运动详情页:与 sport_card 点击行为一致 */
    lv_obj_add_event_cb(calorie_panel, sport_btn_click_cb,
        LV_EVENT_CLICKED, sport_card_inst());

    lv_obj_t *label_top = lv_obj_create(calorie_panel);
    lv_obj_remove_style_all(label_top);
    lv_obj_set_width(label_top, lv_pct(100));
    lv_obj_set_flex_grow(label_top, 1);
    lv_obj_set_flex_flow(label_top, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(label_top, LV_FLEX_ALIGN_SPACE_BETWEEN, 
        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    {
        lv_obj_t *label_life = lv_label_create(label_top);
        lv_label_bind_text(label_life, &g_calorie_subject, "%d/kCal");

        lv_obj_t *label_right = lv_label_create(label_top);
        lv_obj_set_style_text_align(label_right, LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_text(label_right, "calorie");
    }

    lv_obj_t *slider = lv_slider_create(calorie_panel);
    lv_obj_set_width(slider, lv_pct(100));
    lv_obj_set_flex_grow(slider, 3);
    lv_obj_set_style_opa(slider, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_obj_set_style_radius(slider, 4, LV_PART_MAIN);
    lv_obj_set_style_radius(slider, 4, LV_PART_INDICATOR);
    /* 与 sport_card 卡路里圆环同款红色:指示器纯红,背景加深红 */
    lv_obj_set_style_bg_color(slider, lv_color_darken(COLOR_ERROR, LV_OPA_40),
        LV_PART_MAIN);
    lv_obj_set_style_bg_color(slider, COLOR_ERROR, LV_PART_INDICATOR);
    /* 禁止鼠标滑动:slider 仅作进度展示 */
    lv_obj_set_clickable(slider, false);
    /* 满量程与数据层保持一致,数值与卡路里实时同步 */
    lv_slider_set_range(slider, 0, CALORIE_MAX);
    lv_subject_add_observer_obj(&g_calorie_subject, calorie_slider_obs_cb,
        slider, NULL);

    lv_obj_t *label_time = lv_obj_create(calorie_panel);
    lv_obj_remove_style_all(label_time);
    lv_obj_set_width(label_time, lv_pct(100));
    lv_obj_set_flex_grow(label_time, 1);
    lv_obj_set_flex_flow(label_time, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(label_time, LV_FLEX_ALIGN_SPACE_BETWEEN, 
        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    {
        lv_obj_t *_00 = lv_label_create(label_time);
        lv_obj_set_style_opa(_00, LV_OPA_50, 0);
        lv_label_set_text(_00, "0");
        lv_obj_t *_24 = lv_label_create(label_time);
        lv_obj_set_style_opa(_24, LV_OPA_50, 0);
        lv_obj_set_style_text_align(_24, LV_TEXT_ALIGN_RIGHT, 0);
        lv_label_set_text(_24, "24");
    }
}

/* 卡路里更新时同步滑块位置 */
static void calorie_slider_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *slider = lv_observer_get_target(obs);
    lv_slider_set_value(slider, lv_subject_get_int(sub), LV_ANIM_OFF);
}

/* 信息面板:SD卡/定位/电量 三个状态按钮 */
static void info_panel_create(lv_obj_t *parent)
{
    lv_obj_t *info_panel = lv_obj_create(parent);
    lv_obj_set_width(info_panel, lv_pct(100));
    lv_obj_set_flex_grow(info_panel, 3);
    lv_obj_set_flex_flow(info_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(info_panel,
                      LV_FLEX_ALIGN_START,
                      LV_FLEX_ALIGN_CENTER,
                      LV_FLEX_ALIGN_START);
    {
        lv_obj_t *temp = lv_obj_create(info_panel);
        lv_obj_set_flex_grow(temp, 1);
        lv_obj_set_height(temp, lv_pct(100));
        lv_obj_t *sim_label = lv_label_create(temp);
        lv_label_set_text(sim_label, LV_SYMBOL_SD_CARD);
        lv_obj_add_style(sim_label, &info_style, 0);
        lv_obj_center(sim_label);

        lv_obj_t *compass = lv_obj_create(info_panel);
        lv_obj_set_style_pad_all(compass, 4, 0);
        lv_obj_set_flex_grow(compass, 1);
        lv_obj_set_height(compass, lv_pct(100));
        /* 点击进入指南针详情页:与 sport 详情页打开逻辑一致 */
        lv_obj_add_event_cb(compass, compass_btn_click_cb,
            LV_EVENT_CLICKED, sport_card_inst());
        {
            /* 中心航向数值挂在 compass 下,不随罗盘旋转,始终正立 */
            lv_obj_t *compass_center_label = lv_label_create(compass);
            lv_obj_center(compass_center_label);
            lv_obj_add_style(compass_center_label, &info_style, 0);
            /* 中心航向数值由数据层 g_compass_subject 驱动 */
            lv_label_bind_text(compass_center_label, &g_compass_subject, "%d°");

            /* 罗盘组件:4 段弧 + N/E/S/W,随航向整体旋转 */
            static compass_widget_t compass_w;
            static const char *compass_dir[] = {"N", "E", "S", "W"};
            static const float compass_angle[] = {270, 0, 90, 180};
            compass_cfg_t cfg = {
                .heading = &g_compass_subject,
                .dir = compass_dir,
                .dir_angle = compass_angle,
                .n = 4,
                .arc_width = 4,
                .arc_gap = 32,
                .dir_font = &lv_font_montserrat_12,
                .north_color = COLOR_ERROR,
                .dir_color = COLOR_PRIMARY,
                .radius_pct = 100,
                .radius_inset = 8,
                .label_facing_center = false,
            };
            compass_widget_create(&compass_w, compass, &cfg);

            /* 子对象事件冒泡,确保整个罗盘区域可点击 */
            enable_bubble_all(compass);
        }

        lv_obj_t *BAT = lv_obj_create(info_panel);
        lv_obj_set_style_pad_all(BAT, 4, 0);
        lv_obj_set_flex_grow(BAT, 1);
        lv_obj_set_height(BAT, lv_pct(100));
        {
            lv_obj_t *BAT_label = lv_label_create(BAT);
            // lv_label_set_text(BAT_label, LV_SYMBOL_BATTERY_1);
            lv_obj_add_style(BAT_label, &info_style, 0);
            lv_obj_center(BAT_label);
            lv_obj_set_scrollbar_mode(BAT, LV_SCROLLBAR_MODE_OFF);
            /* 电量数值由 g_bat_subject 驱动 */
            lv_label_bind_text(BAT_label, &g_bat_subject, "%d");

            BAT_arc_create(BAT);

            /* 电量图标:位于 BAT 正下方,分级规则与 core 一致 */
            lv_obj_t *BAT_icon_label = lv_label_create(BAT);
            lv_obj_add_style(BAT_icon_label, &info_style, 0);
            lv_obj_set_align(BAT_icon_label, LV_ALIGN_BOTTOM_MID);
            lv_label_set_text(BAT_icon_label,
                bat_symbol_level(lv_subject_get_int(&g_bat_subject)));
            lv_subject_add_observer(&g_bat_subject, bat_icon_obs_cb,
                BAT_icon_label);
        }
    }
}

typedef struct 
{
    lv_obj_t *arc[3];
    int16_t seg_start_angle[3];
    int16_t seg_min[3];   /* 每段弧对应的电量最小值 */
    int16_t seg_max[3];   /* 每段弧对应的电量最大值 */
}bat_arc_t;
/* 三段电量区间:低 / 中 / 高,边界取自 data_center 电量分档 */
static bat_arc_t bat_arc_ring = {
    .seg_min = {0, BAT_LEVEL_LOW_MAX + 1, BAT_LEVEL_MID_MAX + 1},
    .seg_max = {BAT_LEVEL_LOW_MAX, BAT_LEVEL_MID_MAX, BAT_LEVEL_FULL},
};

/* 按电量刷新三段弧环:每段 value = 电量在该区间的偏移 */
static void bat_arc_sync(int16_t bat)
{
    for (size_t i = 0; i < 3; i++)
    {
        int16_t seg_len = bat_arc_ring.seg_max[i] - bat_arc_ring.seg_min[i];
        int16_t value = bat - bat_arc_ring.seg_min[i];
        if (value < 0) value = 0;
        if (value > seg_len) value = seg_len;
        lv_arc_set_value(bat_arc_ring.arc[i], value);
    }
}

/* 电量 subject 观察者:变化时同步弧环 */
static void bat_arc_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    bat_arc_sync(lv_subject_get_int(sub));
}

static void BAT_arc_create(lv_obj_t *parent)
{
    lv_obj_t *obj = parent;

    for (size_t i = 0; i < 3; i++)
    {
        bat_arc_ring.seg_start_angle[i] = ((i * 90)) + 90 + 45 + 9;
    }
    for (size_t i = 0; i < 3; i++)
    {
        bat_arc_ring.arc[i] = lv_arc_create(obj);
        lv_obj_set_size(bat_arc_ring.arc[i], lv_pct(100), lv_pct(100));
        lv_obj_center(bat_arc_ring.arc[i]);
        lv_arc_set_bg_angles(bat_arc_ring.arc[i], 
            bat_arc_ring.seg_start_angle[i], 
            bat_arc_ring.seg_start_angle[i] + 72);
        lv_obj_set_style_arc_width(bat_arc_ring.arc[i], 4, LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(bat_arc_ring.arc[i], 4, LV_PART_MAIN);
        lv_obj_remove_style(bat_arc_ring.arc[i], NULL, LV_PART_KNOB);
        /* 每段弧 value 范围 = 该段电量区间长度 */
        lv_arc_set_range(bat_arc_ring.arc[i], 0,
            bat_arc_ring.seg_max[i] - bat_arc_ring.seg_min[i]);
        lv_arc_set_value(bat_arc_ring.arc[i], 0);
    }

    /* 绑定电量 subject,并立即同步一次当前电量 */
    lv_subject_add_observer(&g_bat_subject, bat_arc_obs_cb, NULL);
    bat_arc_sync(lv_subject_get_int(&g_bat_subject));
}

/* 电量变化时刷新图标 */
static void bat_icon_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *label = lv_observer_get_user_data(obs);
    lv_label_set_text(label, bat_symbol_level(lv_subject_get_int(sub)));
}


