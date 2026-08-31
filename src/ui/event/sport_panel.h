#ifndef SPORT_PANEL_H
#define SPORT_PANEL_H

#include "lvgl.h"

/* 创建运动详情页 UI:控件挂在 parent 下,导航/手势由 ScreenNavigator 处理 */
void sport_panel_create(lv_obj_t *parent);

#endif
