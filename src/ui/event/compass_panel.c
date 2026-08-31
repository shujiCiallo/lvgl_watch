#include "compass_panel.h"
#include "core/data_center.h"
#include "style/app_colors.h"

/* 指南针详情页 UI */
void compass_panel_create(compass_panel_t *self)
{
    self->root = lv_obj_create(NULL);
    lv_obj_remove_style_all(self->root);
    lv_obj_set_size(self->root, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_all(self->root, 8, 0);

    lv_obj_t *label = lv_label_create(self->root);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    lv_obj_center(label);
    /* 中心航向数值由数据层 g_compass_subject 驱动,不随罗盘旋转 */
    lv_label_bind_text(label, &g_compass_subject, "%d°");

    /* 罗盘组件:2 段弧 + S/N,随航向整体旋转 */
    static const char *compass_dir[] = {"S", "N"};
    static const float compass_angle[] = {180, 0};
    compass_cfg_t cfg = {
        .heading = &g_compass_subject,
        .dir = compass_dir,
        .dir_angle = compass_angle,
        .n = 2,
        .arc_width = 24,
        .arc_gap = 32,
        .dir_font = &lv_font_montserrat_28,
        .north_color = COLOR_ERROR,
        .dir_color = lv_color_hex(0x000000),
        .radius_pct = 90,
        .radius_inset = 24,
        .label_facing_center = true,
    };
    compass_widget_create(&self->compass_w, self->root, &cfg);
}
