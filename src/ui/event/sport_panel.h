#ifndef SPORT_PANEL_H
#define SPORT_PANEL_H

#include "lvgl.h"

typedef struct {
    lv_obj_t *root;
}sport_panel_t;

/* 创建运动详情页 UI:自建全屏 root,导航/手势由 ScreenNavigator 处理 */
void sport_panel_create(sport_panel_t *self);

#endif
