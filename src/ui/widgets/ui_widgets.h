#ifndef UI_WIDGETS_H
#define UI_WIDGETS_H

#include "lvgl.h"

/* 通用按钮控件组 */
typedef struct Button{
    lv_obj_t *btn;          /* 按钮对象 */
    lv_obj_t *label;        /* 按钮上的文本标签 */
    const char *text;       /* 标签显示的文本 */
    lv_event_cb_t cb;
}Button_t;

#endif
