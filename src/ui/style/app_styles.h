#ifndef APP_STYLES_H
#define APP_STYLES_H

#include "lvgl/lvgl.h"

/* 全局样式变量 */
extern lv_style_t style_screen_bg;       /* 屏幕背景 */
extern lv_style_t info_style;


/* 初始化所有样式  */
void styles_init(void);

#endif
