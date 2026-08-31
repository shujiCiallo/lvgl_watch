#include "activity_rings.h"

/* 圆环数据观察者回调:把 subject 数值写到 arc */
static void arc_observer_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *arc = lv_observer_get_target(obs);
    lv_arc_set_value(arc, lv_subject_get_int(sub));
}

lv_obj_t *activity_rings_create(lv_obj_t *parent,
    activity_ring_cfg_t *cfgs, size_t n)
{
    for (size_t i = 0; i < n; i++) {
        cfgs[i].arc = lv_arc_create(parent);

        lv_obj_clear_flag(cfgs[i].arc, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(cfgs[i].arc, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_set_event_bubble(cfgs[i].arc, true);
        lv_obj_set_size(cfgs[i].arc, cfgs[i].size, cfgs[i].size);
        lv_obj_align(cfgs[i].arc, LV_ALIGN_BOTTOM_MID, 0, cfgs[i].size / 2);

        lv_obj_set_style_opa(cfgs[i].arc, LV_OPA_TRANSP, LV_PART_KNOB);
        lv_obj_set_style_arc_color(cfgs[i].arc, cfgs[i].color,
            LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(cfgs[i].arc, cfgs[i].width,
            LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(cfgs[i].arc, cfgs[i].width,
            LV_PART_MAIN);
        lv_color_t main_color = lv_color_darken(cfgs[i].color, LV_OPA_40);
        lv_obj_set_style_arc_color(cfgs[i].arc, main_color,
            LV_PART_MAIN);

        lv_arc_set_bg_angles(cfgs[i].arc, 180, 0);
        lv_arc_set_max_value(cfgs[i].arc, cfgs[i].max_value);
        lv_arc_set_value(cfgs[i].arc, 0);

        /* 绑定对象订阅:arc 删除时 observer 自动移除,避免悬空 */
        lv_subject_add_observer_obj(cfgs[i].subject, arc_observer_cb,
            cfgs[i].arc, NULL);
    }
    return parent;
}
