#ifndef SCREEN_CARD_H
#define SCREEN_CARD_H

#include "lvgl.h"

/* 卡片屏幕(运动数据页)运行时状态 */
typedef struct {
    lv_obj_t *root;            /* 屏幕根对象 */
    lv_style_t title_style;    /* 标题栏样式 */
    lv_observer_t *arc_obs[3]; /* 三个活动圆环的订阅句柄,便于统一移除 */
    lv_obj_t *main_scr;        /* 进入详情页前的活动屏幕,右滑返回时恢复 */
    lv_obj_t *sport_scr;       /* 运动详情页根对象 */
} screen_card_t;

/* 创建卡片屏幕:状态写入 self,控件挂在 parent 下 */
void screen_card_create(screen_card_t *self, lv_obj_t *parent);

#endif
