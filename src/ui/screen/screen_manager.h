#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "lvgl.h"
#include "screen/screen_card.h"
#include "widgets/ui_widgets.h"

extern lv_obj_t *g_tileview;
extern int g_tile_index;

/* 创建主 tileview(表盘主界面) */
void screen_tileview_create(lv_obj_t *parent);

/* 运动卡片屏幕单例,供各界面点击跳转 sport 详情页 */
screen_card_t *sport_card_inst(void);

#endif
