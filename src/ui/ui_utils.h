#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "lvgl.h"

/* 让 obj 的所有子孙事件/手势冒泡到 obj;obj 自身不冒泡,以便命中根回调 */
void enable_bubble_all(lv_obj_t *obj);

#endif
