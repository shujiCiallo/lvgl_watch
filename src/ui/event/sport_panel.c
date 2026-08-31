#include "sport_panel.h"
#include "widgets/activity_rings.h"
#include "widgets/stat_label.h"
#include "style/app_colors.h"
#include "core/data_center.h"

/* 运动详情页:活动圆环 + 数值标签,数据均由 data_center 的 subject 驱动 */
void sport_panel_create(sport_panel_t *self)
{
    self->root = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(self->root, 8, 0);
    lv_obj_set_size(self->root, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(self->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollable(self->root, false);

    lv_obj_t *meter = lv_obj_create(self->root);
    lv_obj_remove_style_all(meter);
    lv_obj_set_size(meter, lv_pct(100), lv_pct(45));
    lv_obj_set_scrollable(meter, false);
    activity_ring_cfg_t rings[] = {
        {NULL, 140, 27, CALORIE_MAX, COLOR_ERROR, &g_calorie_subject},
        {NULL, 210, 27, STEPS_MAX, COLOR_PRIMARY_DARK, &g_steps_subject},
        {NULL, 280, 27, DURATION_MAX, COLOR_SECONDARY, &g_duration_subject}
    };
    activity_rings_create(meter, rings, 3);

    lv_obj_t *info = lv_obj_create(self->root);
    lv_obj_remove_style_all(info);
    lv_obj_set_size(info, lv_pct(100), lv_pct(50));
    lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(info, 8, 0);

    stat_label_t label[3];
    const char *titles[] = {"calorie", "steps", "times"};
    lv_subject_t *subs[] = {
        &g_calorie_subject, &g_steps_subject, &g_duration_subject};
    int32_t maxs[] = {CALORIE_MAX, STEPS_MAX, DURATION_MAX};
    for (size_t i = 0; i < 3; i++) {
        stat_label_create(&label[i], info, titles[i], subs[i], maxs[i]);
    }
}
