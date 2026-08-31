#include "compass_panel.h"
#include "core/data_center.h"
#include "style/app_colors.h"
#include <math.h>

static lv_obj_t *compass_ring[2];
static lv_obj_t *compass_panel_rot;   /* 详情页罗盘旋转容器,供后续旋转 */

/* 详情页方向标签定位(仿 screen_main 罗盘,极坐标) */
static lv_obj_t *compass_dir_label[2];
static float compass_dir_angle[2];

#define COMPASS_DIR_DEG2RAD 0.017453292519943295f

/* 方向标签极坐标定位:放在弧缺口中间,文字顶部朝向圆心 */
static void compass_dir_label_pos_set(lv_obj_t *parent, lv_obj_t *label, float angle_deg)
{
    lv_obj_update_layout(parent);
    lv_obj_update_layout(label);

    lv_coord_t cx = lv_obj_get_width(parent) / 2;
    lv_coord_t cy = lv_obj_get_height(parent) / 2;
    /* 弧中心线半径约 0.9*半边长;弧宽 24(半宽 12),标签再向圆心收 12px */
    lv_coord_t r = (lv_coord_t)(LV_MIN(cx, cy) * 0.9f) - 12 - 12;
    float rad = angle_deg * COMPASS_DIR_DEG2RAD;
    lv_coord_t x = cx + (lv_coord_t)(r * cosf(rad)) - lv_obj_get_width(label) / 2;
    lv_coord_t y = cy + (lv_coord_t)(r * sinf(rad)) - lv_obj_get_height(label) / 2;
    lv_obj_set_pos(label, x, y);
}

/* 旋转容器尺寸最终确定后重算方向标签位置 */
static void compass_dir_repos_cb(lv_event_t *e)
{
    lv_obj_t *parent = lv_event_get_target(e);
    for (size_t i = 0; i < 2; i++) {
        if (compass_dir_label[i] != NULL) {
            compass_dir_label_pos_set(parent, compass_dir_label[i], compass_dir_angle[i]);
        }
    }
}

/* 指南针详情页 UI */
void compass_panel_create(lv_obj_t *parent)
{
    lv_obj_t *obj = parent;

    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(obj, 8, 0);

    lv_obj_t *label = lv_label_create(obj);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_center(label);
    /* 中心航向数值由数据层 g_compass_subject 驱动 */
    lv_label_bind_text(label, &g_compass_subject, "%d°");

    /* 旋转容器:罗盘(弧+方向标签)绕中心整体旋转,标签随之公转 */
    compass_panel_rot = lv_obj_create(obj);
    lv_obj_remove_style_all(compass_panel_rot);
    lv_obj_set_size(compass_panel_rot, lv_pct(100), lv_pct(100));
    lv_obj_center(compass_panel_rot);
    lv_obj_set_scrollable(compass_panel_rot, false);
    lv_obj_set_style_transform_pivot_x(compass_panel_rot, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(compass_panel_rot, lv_pct(50), 0);

    for (size_t i = 0; i < 2; i++){
        compass_ring[i]  = lv_arc_create(compass_panel_rot);
        lv_obj_center(compass_ring[i]);
        lv_obj_set_width(compass_ring[i], lv_pct(90));
        lv_coord_t h = lv_obj_get_width(obj);
        lv_obj_set_height(compass_ring[i], h * 0.9);
        lv_arc_set_bg_angles(compass_ring[i],
            (i * 180) + 32/2, ((i+1) * 180) - 32/2);
        lv_arc_set_angles(compass_ring[i],
            (i * 180) + 32/2,
            ((i+1) * 180) - 32/2);
        lv_obj_remove_style(compass_ring[i], NULL, LV_PART_KNOB);
        lv_obj_clear_flag(compass_ring[i], LV_OBJ_FLAG_CLICKABLE);
        lv_arc_set_max_value(compass_ring[i], 180);
        lv_arc_set_value(compass_ring[i], 0);
        lv_obj_set_style_arc_width(compass_ring[i],
            24, LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(compass_ring[i],
            24, LV_PART_MAIN);
    }

    /* 方向标签:左缺口 S(180°),右缺口 N(0°),文字旋转 90° 使顶部朝向圆心 */
    static const char *dir_text[2] = {"S", "N"};
    static const float dir_angle[2] = {180.0f, 0.0f};
    for (size_t i = 0; i < 2; i++) {
        compass_dir_label[i] = lv_label_create(compass_panel_rot);
        lv_label_set_text(compass_dir_label[i], dir_text[i]);
        lv_obj_set_style_text_align(compass_dir_label[i], LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_text_font(compass_dir_label[i], &lv_font_montserrat_28, 0);
        /* N(右)用红色,其余方向用黑色 */
        lv_obj_set_style_text_color(compass_dir_label[i],
            (i == 1) ? COLOR_ERROR : lv_color_hex(0x000000), 0);
        compass_dir_angle[i] = dir_angle[i];
        /* 旋转枢轴=标签中心,避免默认左上角枢轴导致旋转后整体偏移 */
        lv_obj_set_style_transform_pivot_x(compass_dir_label[i], lv_pct(50), 0);
        lv_obj_set_style_transform_pivot_y(compass_dir_label[i], lv_pct(50), 0);
        /* 左侧标签顶朝右(圆心)顺时针 90°,右侧标签顶朝左(圆心)逆时针 90° */
        lv_obj_set_style_transform_rotation(compass_dir_label[i],
            (i == 0) ? 900 : 2700, 0);
        compass_dir_label_pos_set(compass_panel_rot, compass_dir_label[i], compass_dir_angle[i]);
    }
    /* 旋转容器尺寸确定后重算方向标签位置 */
    lv_obj_add_event_cb(compass_panel_rot, compass_dir_repos_cb, LV_EVENT_SIZE_CHANGED, NULL);
}
