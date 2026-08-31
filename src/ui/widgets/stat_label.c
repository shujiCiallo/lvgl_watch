#include "stat_label.h"

/* 数值观察者回调:显示 "当前/满量程";user_data 传数值 label,max 存 label 的 user_data */
static void label_update_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *label = lv_observer_get_user_data(obs);
    int32_t max = (int32_t)(intptr_t)lv_obj_get_user_data(label);
    lv_label_set_text_fmt(label, "%d/%d", lv_subject_get_int(sub), max);
}

void stat_label_create(stat_label_t *label, lv_obj_t *parent,
    const char *title, lv_subject_t *subject, int32_t max)
{
    label->parent = lv_obj_create(parent);
    lv_obj_remove_style_all(label->parent);
    lv_obj_set_size(label->parent, lv_pct(100), lv_pct(30));
    lv_obj_set_flex_flow(label->parent, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(label->parent, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_text_font(label->parent, &lv_font_montserrat_28, 0);

    label->line1 = lv_label_create(label->parent);
    lv_label_set_text(label->line1, title);
    lv_obj_set_flex_grow(label->line1, 3);
    lv_obj_set_style_text_align(label->line1, LV_TEXT_ALIGN_CENTER, 0);

    label->line2 = lv_label_create(label->parent);
    lv_obj_set_flex_grow(label->line2, 4);
    lv_obj_set_style_text_align(label->line2, LV_TEXT_ALIGN_CENTER, 0);

    /* max 存 label 的 user_data,观察者回调读取 */
    lv_obj_set_user_data(label->line2, (void *)(intptr_t)max);
    lv_subject_add_observer_obj(subject, label_update_cb,
        label->line2, label->line2);
}
