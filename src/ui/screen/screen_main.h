#ifndef SCREEN_MAIN_H
#define SCREEN_MAIN_H

#include "lvgl.h"

/* 主屏幕(表盘)运行时状态 */
typedef struct {
    lv_obj_t *root;   /* 屏幕根对象 */
} screen_main_t;

/* 创建主屏幕:状态写入 self,控件挂在 parent 下 */
void screen_main_create(screen_main_t *self, lv_obj_t *parent);

#endif
