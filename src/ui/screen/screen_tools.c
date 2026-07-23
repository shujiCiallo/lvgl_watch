#include "screen_tools.h"
#include "lvgl.h"
#include "event/event.h"
#include "screen_manager.h"

static lv_obj_t *s_tools_panel = NULL;

static void title_create(lv_obj_t* parent);

void screen_tools_create(lv_obj_t *parent)
{

    s_tools_panel = parent;

    static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(3),
        LV_GRID_FR(3), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
    static int32_t col_dsc[] = {LV_GRID_FR(1),LV_GRID_FR(1),
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_layout(s_tools_panel, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(s_tools_panel, col_dsc, row_dsc);
    lv_obj_set_style_pad_row(s_tools_panel, 5, 0);
    lv_obj_set_style_pad_column(s_tools_panel, 5, 0);

    title_create(s_tools_panel);
}

static void title_create(lv_obj_t *parent)
{
    lv_obj_t *bt_label = lv_label_create(parent);
    lv_label_set_text(bt_label, LV_SYMBOL_BLUETOOTH);
    lv_obj_set_grid_cell(bt_label, LV_GRID_ALIGN_STRETCH,
        0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    lv_obj_t *time_label = lv_label_create(parent);
    lv_label_set_text(time_label, time_buf);
    lv_obj_set_grid_cell(time_label, LV_GRID_ALIGN_END,
        1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
}
