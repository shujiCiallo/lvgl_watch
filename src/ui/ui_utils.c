#include "ui_utils.h"

/* 统一冒泡工具:替代各屏幕重复的 gesture_bubble_all / event_bubble_all */
void enable_bubble_all(lv_obj_t *obj)
{
    uint32_t cnt = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(obj, i);
        lv_obj_set_event_bubble(child, true);
        lv_obj_set_gesture_bubble(child, true);
        enable_bubble_all(child);
    }
}
