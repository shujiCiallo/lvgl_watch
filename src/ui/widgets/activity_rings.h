#ifndef ACTIVITY_RINGS_H
#define ACTIVITY_RINGS_H

#include "lvgl.h"

/* 单个活动圆环的创建配置 */
typedef struct {
    lv_obj_t *arc;        /* arc 控件(由组件创建后回填) */
    uint16_t size;        /* 直径 */
    uint16_t width;       /* 弧线宽度 */
    int32_t max_value;    /* 满量程值 */
    lv_color_t color;     /* 指示条颜色 */
    lv_subject_t *subject;/* 数据源 */
} activity_ring_cfg_t;

/* 创建 n 个同心活动圆环,每个订阅各自 subject;返回 parent */
lv_obj_t *activity_rings_create(lv_obj_t *parent,
    activity_ring_cfg_t *cfgs, size_t n);

#endif
