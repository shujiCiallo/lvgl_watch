#ifndef SCREEN_APP_H
#define SCREEN_APP_H

#include "lvgl.h"

/* 应用列表屏幕运行时状态 */
typedef struct {
    lv_obj_t *root;   /* 屏幕根对象 */
} screen_app_t;

/* 创建应用列表屏幕:状态写入 self,控件挂在 parent 下 */
void screen_app_create(screen_app_t *self, lv_obj_t *parent);

#endif
