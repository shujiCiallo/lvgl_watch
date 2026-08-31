#ifndef COMPASS_PANEL_H
#define COMPASS_PANEL_H

#include "lvgl.h"

/* 创建指南针详情页 UI:控件挂在 parent 下,导航/手势由 ScreenNavigator 处理 */
void compass_panel_create(lv_obj_t *parent);

#endif
