#include "compass_widget.h"
#include <math.h>

#define COMPASS_DEG2RAD 0.017453292519943295f

/* 方向标签极坐标定位 */
static void dir_label_pos_set(compass_widget_t *self, lv_obj_t *label, float angle_deg)
{
    lv_obj_t *parent = self->rot;
    lv_obj_update_layout(parent);
    lv_obj_update_layout(label);

    lv_coord_t cx = lv_obj_get_width(parent) / 2;
    lv_coord_t cy = lv_obj_get_height(parent) / 2;
    lv_coord_t r = (lv_coord_t)(LV_MIN(cx, cy) * self->radius_pct / 100)
        - self->radius_inset;
    float rad = angle_deg * COMPASS_DEG2RAD;
    lv_coord_t x = cx + (lv_coord_t)(r * cosf(rad)) - lv_obj_get_width(label) / 2;
    lv_coord_t y = cy + (lv_coord_t)(r * sinf(rad)) - lv_obj_get_height(label) / 2;
    lv_obj_set_pos(label, x, y);
}

/* 旋转容器尺寸确定后重算标签位置 */
static void compass_repos_cb(lv_event_t *e)
{
    compass_widget_t *self = lv_event_get_user_data(e);
    lv_obj_t *parent = lv_event_get_target(e);
    for (size_t i = 0; i < self->n; i++) {
        if (self->dir_label[i] != NULL) {
            dir_label_pos_set(self, self->dir_label[i], self->dir_angle[i]);
        }
    }
}

/* 航向变化时整体旋转罗盘(0.1° 为单位) */
static void heading_obs_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    compass_widget_t *self = lv_observer_get_user_data(obs);
    if (self == NULL || self->rot == NULL) return;
    lv_obj_set_style_transform_rotation(self->rot,
        lv_subject_get_int(sub) * 10, 0);
}

void compass_widget_create(compass_widget_t *self, lv_obj_t *parent,
    const compass_cfg_t *cfg)
{
    self->n = cfg->n;
    self->radius_pct = cfg->radius_pct;
    self->radius_inset = cfg->radius_inset;
    self->label_facing_center = cfg->label_facing_center;
    for (size_t i = 0; i < COMPASS_DIR_MAX; i++) self->dir_label[i] = NULL;

    /* 透明旋转容器:弧与标签都挂在其下整体旋转 */
    self->rot = lv_obj_create(parent);
    lv_obj_remove_style_all(self->rot);
    lv_obj_set_size(self->rot, lv_pct(100), lv_pct(100));
    lv_obj_center(self->rot);
    lv_obj_set_scrollable(self->rot, false);
    lv_obj_set_style_transform_pivot_x(self->rot, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(self->rot, lv_pct(50), 0);

    /* 弧段:等分 360,每段两端留 arc_gap/2 缺口,全亮作圆环装饰 */
    uint16_t seg = 360 / cfg->n;
    for (size_t i = 0; i < cfg->n; i++) {
        lv_obj_t *arc = lv_arc_create(self->rot);
        lv_obj_set_size(arc, lv_pct(100), lv_pct(100));
        lv_obj_center(arc);
        lv_arc_set_bg_angles(arc,
            (int16_t)(i * seg + cfg->arc_gap / 2),
            (int16_t)((i + 1) * seg - cfg->arc_gap / 2));
        lv_obj_set_style_arc_width(arc, cfg->arc_width, LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(arc, cfg->arc_width, LV_PART_MAIN);
        lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
        lv_arc_set_range(arc, 0, seg);
        lv_arc_set_value(arc, seg);
        lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    }

    /* 方向标签:极坐标放在弧缺口处,可选旋转使顶部朝向圆心 */
    for (size_t i = 0; i < cfg->n; i++) {
        lv_obj_t *label = lv_label_create(self->rot);
        lv_label_set_text(label, cfg->dir[i]);
        lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(label, cfg->dir_font, 0);
        lv_obj_set_style_text_color(label,
            (i == 0) ? cfg->north_color : cfg->dir_color, 0);
        self->dir_angle[i] = cfg->dir_angle[i];
        if (cfg->label_facing_center) {
            lv_obj_set_style_transform_pivot_x(label, lv_pct(50), 0);
            lv_obj_set_style_transform_pivot_y(label, lv_pct(50), 0);
            /* 左半标签顶朝右(顺时针 90°),右半顶朝左(逆时针 90°) */
            lv_obj_set_style_transform_rotation(label,
                (cfg->dir_angle[i] >= 180) ? 900 : 2700, 0);
        }
        self->dir_label[i] = label;
        dir_label_pos_set(self, label, cfg->dir_angle[i]);
    }

    /* 旋转容器尺寸确定后重算标签位置 */
    lv_obj_add_event_cb(self->rot, compass_repos_cb,
        LV_EVENT_SIZE_CHANGED, self);

    /* 航向数据驱动整体旋转;绑定到 rot,页面销毁时 observer 自动移除 */
    if (cfg->heading != NULL) {
        lv_subject_add_observer_obj(cfg->heading, heading_obs_cb,
            self->rot, self);
    }
}
