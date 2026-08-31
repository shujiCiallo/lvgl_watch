#ifndef COMPASS_PANEL_H
#define COMPASS_PANEL_H

#include "lvgl.h"
#include "widgets/compass_widget.h"

typedef struct {
    lv_obj_t *root;
    compass_widget_t compass_w;   /* 罗盘组件状态,随页面持有 */
}compass_panel_t;

/* 创建指南针详情页 UI:自建全屏 root,导航/手势由 ScreenNavigator 处理 */
void compass_panel_create(compass_panel_t *self);

#endif
