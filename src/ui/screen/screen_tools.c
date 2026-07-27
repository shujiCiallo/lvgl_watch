#include "screen_tools.h"
#include "lvgl.h"
#include "event/event.h"
#include "screen_manager.h"

static lv_obj_t *s_tools_panel = NULL;

static void title_create(lv_obj_t* parent);
static void tools_create(lv_obj_t *parent);

typedef struct tools {
    lv_obj_t *button;
    lv_obj_t *label;
    const char *text;
}tools_t;

void screen_tools_create(lv_obj_t *parent)
{

    s_tools_panel = parent;

    static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(3),
        LV_GRID_FR(3), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
    static int32_t col_dsc[] = {LV_GRID_FR(1),LV_GRID_FR(1),
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_layout(s_tools_panel, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(s_tools_panel, col_dsc, row_dsc);
    lv_obj_set_style_pad_row(s_tools_panel, 8, 0);
    lv_obj_set_style_pad_column(s_tools_panel, 8, 0);
    lv_obj_set_style_pad_all(parent, 10, 0);
    title_create(s_tools_panel);
    tools_create(s_tools_panel);
}

static lv_style_t title_style;

static void title_create(lv_obj_t *parent)
{
    lv_style_init(&title_style);
    lv_style_set_bg_opa(&title_style, LV_OPA_COVER);
    // lv_style_set_bg_color(&title_style, lv_color_hex(0x1234f234));
    lv_style_set_text_font(&title_style, &lv_font_montserrat_36);

    lv_obj_t *bt_label = lv_label_create(parent);
    lv_label_set_text(bt_label, LV_SYMBOL_BLUETOOTH);
    lv_obj_add_style(bt_label, &title_style, 0);
    lv_obj_set_grid_cell(bt_label, LV_GRID_ALIGN_START,
        0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_t *bat_label = lv_label_create(parent);
    lv_obj_add_style(bat_label, &title_style, 0);
    lv_label_set_text(bat_label, LV_SYMBOL_BATTERY_1);
    lv_obj_set_grid_cell(bat_label, LV_GRID_ALIGN_END,
        0, 1,LV_GRID_ALIGN_STRETCH , 0, 1);

    lv_obj_t *time_label = lv_label_create(parent);
    lv_obj_add_style(time_label, &title_style, 0);
    lv_label_set_text(time_label, time_buf);
    lv_obj_set_grid_cell(time_label, LV_GRID_ALIGN_END,
        1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_timer_create(time_cb, 1000, time_label);

}

lv_style_t tools_style;

static void tools_create(lv_obj_t *parent)
{
    static tools_t tools_buf[] = {
        {NULL, NULL, LV_SYMBOL_SD_CARD},
        {NULL, NULL, LV_SYMBOL_USB},
        {NULL, NULL, LV_SYMBOL_SETTINGS},
        {NULL, NULL, LV_SYMBOL_TRASH},
        {NULL, NULL, LV_SYMBOL_DRIVE},
        {NULL, NULL, LV_SYMBOL_FILE}
    };

    lv_style_init(&tools_style);
    lv_style_set_text_font(&tools_style, &lv_font_montserrat_48);

    size_t num = sizeof(tools_buf) / sizeof(tools_buf[0]);
    for (size_t i = 0; i < num; i++) {
        int col = i % 2;
        int row = i / 2;
        tools_buf[i].button =  lv_btn_create(parent);
        tools_buf[i].label = lv_label_create(tools_buf[i].button);
        lv_obj_center(tools_buf[i].label);
        lv_obj_add_style(tools_buf[i].label, &tools_style, 0);
        lv_label_set_text(tools_buf[i].label, tools_buf[i].text);
        lv_obj_set_grid_cell(tools_buf[i].button, LV_GRID_ALIGN_STRETCH,
                            col, 1, LV_GRID_ALIGN_STRETCH,
                            row + 1, 1);
    }


}
