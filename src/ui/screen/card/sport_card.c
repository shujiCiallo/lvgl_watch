#include "sport_card.h"
#include "event/event.h"
#include "style/app_colors.h"
#include "style/app_styles.h"
#include "core/data_center.h"
#include "widgets/activity_rings.h"

/* 数据标签组:一行标题 + 一行数值 */
typedef struct label_data {
    lv_obj_t *parent;  /* 容器 */
    lv_obj_t *line1;   /* 标题行 */
    lv_obj_t *line2;   /* 数值行 */
} label_data_t;

/* 三个活动圆环各自订阅的数据源 */
static lv_subject_t *data[] = {
    &g_calorie_subject,
    &g_steps_subject,
    &g_duration_subject};

/* 模块内部构建函数 */
static lv_obj_t *label_data_create(lv_obj_t *parent);

/* 运动数据卡片:活动圆环 + 下方数值区 */
lv_obj_t *sport_card_create(screen_card_t *self, lv_obj_t *parent)
{
    lv_obj_t *sport_card = lv_obj_create(parent);
    lv_obj_set_style_bg_color(sport_card, COLOR_SURFACE, 0);
    lv_obj_set_flex_flow(sport_card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sport_card, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_coord_t scr_h = lv_disp_get_ver_res(NULL);
    lv_obj_set_size(sport_card, lv_pct(100), (scr_h * 45 / 100));
    lv_obj_add_event_cb(sport_card, sport_btn_click_cb,
        LV_EVENT_CLICKED, self);
    {
        lv_obj_t *meter = lv_btn_create(sport_card);
        lv_obj_set_scrollable(meter, false);
        lv_obj_remove_style_all(meter);
        lv_obj_set_event_bubble(meter, true);
        lv_obj_set_flex_grow(meter, 1);
        activity_ring_cfg_t cfgs[] = {
            {NULL, 60, 10, CALORIE_MAX, COLOR_ERROR, &g_calorie_subject},
            {NULL, 90, 10, STEPS_MAX, COLOR_PRIMARY_DARK, &g_steps_subject},
            {NULL, 120, 10, DURATION_MAX, COLOR_SECONDARY, &g_duration_subject}
        };
        activity_rings_create(meter, cfgs, 3);

        lv_obj_t *info = lv_obj_create(sport_card);
        lv_obj_remove_style_all(info);
        lv_obj_set_event_bubble(info, true);
        lv_obj_set_flex_grow(info, 1);
        lv_obj_set_width(info, lv_pct(100));
        lv_obj_set_flex_flow(info, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(info, LV_FLEX_ALIGN_SPACE_EVENLY,
            LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_text_color(info, lv_color_black(), 0);
        lv_obj_set_style_text_font(info, &lv_font_montserrat_18, 0);
        lv_obj_set_style_pad_all(info, 8, 0);

        label_data_create(info);
    }
    return sport_card;
}

/* 数值信息区:卡路里/步数/次数 三列数据 */
static lv_obj_t *label_data_create(lv_obj_t *parent)
{
    label_data_t label[3] = {};
    size_t num = sizeof(label) / sizeof(label[0]);
    char *title[] = {"calorie", "steps", "times"};
    for (size_t i = 0; i < num; i++) {
        label[i].parent = lv_btn_create(parent);
        lv_obj_remove_style_all(label[i].parent);
        lv_obj_set_flex_grow(label[i].parent, 1);
        lv_obj_set_height(label[i].parent, lv_pct(100));
        lv_obj_set_flex_flow(label[i].parent, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(label[i].parent, LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_set_event_bubble(label[i].parent, true);

        /* 标题行 */
        label[i].line1 = lv_label_create(label[i].parent);
        lv_obj_set_style_bg_opa(label[i].line1, LV_OPA_COVER, 0);
        lv_obj_set_flex_grow(label[i].line1, 1);
        lv_obj_set_height(label[i].line1, lv_pct(100));
        lv_obj_set_width(label[i].line1, lv_pct(100));
        lv_obj_set_style_text_align(label[i].line1, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(label[i].line1, title[i]);
        lv_obj_set_event_bubble(label[i].line1, true);

        /* 数值行:绑定 subject,数据更新自动刷新 */
        label[i].line2 = lv_label_create(label[i].parent);
        lv_obj_set_style_bg_opa(label[i].line2, LV_OPA_COVER, 0);
        lv_obj_set_flex_grow(label[i].line2, 1);
        lv_obj_set_height(label[i].line2, lv_pct(100));
        lv_obj_set_width(label[i].line2, lv_pct(100));
        lv_obj_set_style_text_align(label[i].line2, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_bind_text(label[i].line2, data[i], "%d");
        lv_obj_set_event_bubble(label[i].line2, true);
    }
    return parent;
}
