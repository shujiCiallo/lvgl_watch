#ifndef COMPASS_WIDGET_H
#define COMPASS_WIDGET_H

#include "lvgl.h"

#define COMPASS_DIR_MAX 4   /* 最多方向数(表盘 4 / 详情页 2) */

/* 罗盘创建配置:弧段 + 方向标签 + 航向驱动 */
typedef struct {
    lv_subject_t *heading;        /* 航向数据源,变化时整体旋转;NULL 则静止 */
    const char *const *dir;       /* 方向文本数组,如 {"N","E","S","W"} */
    const float *dir_angle;       /* 各方向极角(度),0° 在右,顺时针为正 */
    size_t n;                     /* 方向数=弧段数,须 <= COMPASS_DIR_MAX */
    uint16_t arc_width;           /* 弧线宽度 */
    uint16_t arc_gap;             /* 每段弧两端缺口角度(度) */
    const lv_font_t *dir_font;    /* 方向标签字体 */
    lv_color_t north_color;       /* 首个方向颜色 */
    lv_color_t dir_color;         /* 其余方向颜色 */
    uint16_t radius_pct;          /* 标签半径 = min 半边 * pct / 100 */
    uint16_t radius_inset;        /* 标签再向圆心收的像素 */
    bool label_facing_center;     /* true:标签旋转使文字顶部朝向圆心 */
} compass_cfg_t;

/* 罗盘运行时状态(调用者持有,组件填充) */
typedef struct {
    lv_obj_t *rot;                           /* 透明旋转容器 */
    size_t n;
    lv_obj_t *dir_label[COMPASS_DIR_MAX];
    float dir_angle[COMPASS_DIR_MAX];
    uint16_t radius_pct;
    uint16_t radius_inset;
    bool label_facing_center;
} compass_widget_t;

/* 创建罗盘:弧段与方向标签挂在 rot 下整体旋转;状态写入 self */
void compass_widget_create(compass_widget_t *self, lv_obj_t *parent,
    const compass_cfg_t *cfg);

#endif
