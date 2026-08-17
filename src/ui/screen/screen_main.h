#ifndef __SCREEN_MAIN_H
#define __SCREEN_MAIN_H

#include "lvgl.h"

/* 主屏幕(表盘)运行时状态 */
typedef struct {
    lv_obj_t *root;   /* 屏幕根对象 */
} screen_main_t;

/* 创建主屏幕:状态写入 self,控件挂在 parent 下 */
void screen_main_create(screen_main_t *self, lv_obj_t *parent);

/* 整体旋转罗盘环,angle_deg 为顺时针角度(0.1° 精度) */
void compass_rotate(int angle_deg);

#endif
