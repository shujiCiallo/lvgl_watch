#ifndef STAT_LABEL_H
#define STAT_LABEL_H

#include "lvgl.h"

/* 一行标题 + 一行数值,数值显示 "当前/满量程",订阅 subject 自动刷新 */
typedef struct {
    lv_obj_t *parent;
    lv_obj_t *line1;   /* 标题行 */
    lv_obj_t *line2;   /* 数值行 */
} stat_label_t;

void stat_label_create(stat_label_t *label, lv_obj_t *parent,
    const char *title, lv_subject_t *subject, int32_t max);

#endif
