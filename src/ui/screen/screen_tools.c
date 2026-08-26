#include "screen_tools.h"
#include "lvgl.h"
#include "event/event.h"
#include "screen_manager.h"
#include "core/data_center.h"
#include "style/app_colors.h"
#include "event/setting_page.h"

/* 内部构建函数 */
static void title_create(lv_obj_t *parent);
static void tools_create(lv_obj_t *parent);

static void bat_observer_cb(lv_observer_t *obs,lv_subject_t *sub);

/* 创建工具屏幕:grid 布局的标题栏 + 工具按钮区 */
void screen_tools_create(screen_tools_t *self, lv_obj_t *parent)
{
    self->root = parent;   /* 保存根对象到结构体 */

    /* grid 布局:两列,标题一行 + 工具三行 */
    static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(3),
        LV_GRID_FR(3), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
    static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1),
        LV_GRID_TEMPLATE_LAST};
    lv_obj_set_layout(self->root, LV_LAYOUT_GRID);
    lv_obj_set_grid_dsc_array(self->root, col_dsc, row_dsc);
    lv_obj_set_style_pad_row(self->root, 8, 0);
    lv_obj_set_style_pad_column(self->root, 8, 0);
    lv_obj_set_style_pad_all(self->root, 10, 0);
    title_create(self->root);
    tools_create(self->root);
}

static lv_style_t title_style;   /* 标题栏文本样式,模块级单例 */

/* 标题栏:蓝牙 / 电量 / 时间 */
static void title_create(lv_obj_t *parent)
{
    lv_style_init(&title_style);
    lv_style_set_bg_opa(&title_style, LV_OPA_COVER);
    lv_style_set_text_font(&title_style, &lv_font_montserrat_34);

    lv_obj_t *bt_label = lv_label_create(parent);
    lv_label_set_text(bt_label, LV_SYMBOL_BLUETOOTH);
    lv_obj_add_style(bt_label, &title_style, 0);
    lv_obj_set_grid_cell(bt_label, LV_GRID_ALIGN_END,
        0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

    lv_obj_t *bat_label = lv_label_create(parent);
    lv_obj_add_style(bat_label, &title_style, 0);
    lv_obj_set_grid_cell(bat_label, LV_GRID_ALIGN_START,
        0, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_label_set_text(bat_label, "100" LV_SYMBOL_BATTERY_FULL);
    lv_subject_add_observer(&g_bat_subject, bat_observer_cb,
        bat_label);

    lv_obj_t *time_label = lv_label_create(parent);
    lv_obj_add_style(time_label, &title_style, 0);
    lv_label_bind_text(time_label, &g_time_subject, "%s");
    lv_obj_set_grid_cell(time_label, LV_GRID_ALIGN_END,
        1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
}

/* 工具按钮区:6 个常用工具按两列排布 */
static void tools_create(lv_obj_t *parent)
{
    static lv_style_t tools_style;   /* 工具图标样式,仅本函数使用 */

    /* 复用通用 Button_t 配置数组批量创建按钮 */
    static Button_t tools_buf[] = {
        {NULL, NULL, LV_SYMBOL_SD_CARD},
        {NULL, NULL, LV_SYMBOL_USB},
        {NULL, NULL, LV_SYMBOL_SETTINGS},
        {NULL, NULL, LV_SYMBOL_TRASH},
        {NULL, NULL, LV_SYMBOL_DRIVE},
        {NULL, NULL, LV_SYMBOL_FILE}
    };

    lv_style_init(&tools_style);
    lv_style_set_text_font(&tools_style, &lv_font_montserrat_48);
    lv_style_set_text_color(&tools_style, lv_color_black());
    // lv_style_set_bg_color(&tools_style, COLOR_BACKGROUND);

    size_t num = sizeof(tools_buf) / sizeof(tools_buf[0]);
    for (size_t i = 0; i < num; i++) {
        int col = i % 2;
        int row = i / 2;
        tools_buf[i].btn = lv_btn_create(parent);
        tools_buf[i].label = lv_label_create(tools_buf[i].btn);
        lv_obj_center(tools_buf[i].label);
        lv_obj_add_style(tools_buf[i].label, &tools_style, 0);
        lv_label_set_text(tools_buf[i].label, tools_buf[i].text);
        lv_obj_set_style_bg_color(tools_buf[i].btn, COLOR_SURFACE, 0);
        lv_obj_set_grid_cell(tools_buf[i].btn, LV_GRID_ALIGN_STRETCH,
                            col, 1, LV_GRID_ALIGN_STRETCH,
                            row + 1, 1);
    }
    lv_obj_add_event_cb(tools_buf[2].btn, setting_btn_click_cb, 
        LV_EVENT_CLICKED, NULL);

}

static void bat_observer_cb(lv_observer_t *obs,
    lv_subject_t *sub)
{
    uint8_t bat = lv_subject_get_int(&g_bat_subject);
    const char *symbol_bat;
    if (bat >= 100) {
        symbol_bat =  LV_SYMBOL_BATTERY_FULL;
    } else if (bat >= 64) {
        symbol_bat =  LV_SYMBOL_BATTERY_3;
    } else if (bat >= 34) {
        symbol_bat =  LV_SYMBOL_BATTERY_2;
    } else {
        symbol_bat =  LV_SYMBOL_BATTERY_1;
    }

    char bat_buf[24];
    lv_snprintf(bat_buf, sizeof(bat_buf), "%d%s",
        bat, symbol_bat);
    lv_obj_t *label = lv_observer_get_user_data(obs);
    lv_label_set_text(label, bat_buf);
}
