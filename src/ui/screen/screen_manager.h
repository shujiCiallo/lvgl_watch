#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "lvgl.h"

extern lv_obj_t *g_tileview;

/* 通用按钮控件组:一个按钮 + 其上文本标签 */
typedef struct Button{
    lv_obj_t *btn;        /* 按钮对象 */
    lv_obj_t *label;      /* 按钮上的文本标签 */
    const char *text;     /* 标签显示的文本 */
}Button_t;

/* 创建主 tileview(表盘主界面) */
void screen_tileview_create(lv_obj_t *parent);

#endif
