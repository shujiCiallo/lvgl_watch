#ifndef SCREEN_TOOLS_H
#define SCREEN_TOOLS_H

#include "lvgl.h"

/* 工具屏幕运行时状态 */
typedef struct {
    lv_obj_t *root;   /* 屏幕根对象 */
} screen_tools_t;

/* 创建工具屏幕:状态写入 self,控件挂在 parent 下 */
void screen_tools_create(screen_tools_t *self, lv_obj_t *parent);

#endif
