#include "lvgl.h"
#include "screen_app.h"
#include "style/app_styles.h"
#include "style/app_colors.h"
#include "screen/screen_manager.h"

/* 应用列表构建函数,只在模块内部使用 */
static void applist_create(lv_obj_t *parent);

/* 创建应用列表屏幕:纵向排列的应用按钮网格 */
void screen_app_create(screen_app_t *self, lv_obj_t *parent)
{
    self->root = parent;   /* 保存根对象到结构体 */
    lv_obj_add_style(self->root, &style_screen_bg, 0);
    lv_obj_set_flex_flow(self->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_column(self->root, 5, 0);
    lv_obj_set_style_pad_row(self->root, 5, 0);

    applist_create(self->root);
}

static lv_style_t applist_style;   /* 应用按钮文本样式,模块级单例 */

/* 应用列表:用 Button_t 配置数组批量创建 10 个应用按钮 */
static void applist_create(lv_obj_t *parent)
{
    lv_style_init(&applist_style);
    lv_style_set_text_font(&applist_style, &lv_font_montserrat_24);
    lv_style_set_bg_color(&applist_style, COLOR_BACKGROUND);
    lv_style_set_text_color(&applist_style, lv_color_black());

    Button_t applist[] = {
        {NULL, NULL, LV_SYMBOL_VOLUME_MID " VOLUME"},
        {NULL, NULL, LV_SYMBOL_VIDEO " VIDEO"},
        {NULL, NULL, LV_SYMBOL_IMAGE " IMAGE"},
        {NULL, NULL, LV_SYMBOL_CALL " CALL"},
        {NULL, NULL, LV_SYMBOL_WIFI " WIFI"},
        {NULL, NULL, LV_SYMBOL_BLUETOOTH " BLUETOOTH"},
        {NULL, NULL, LV_SYMBOL_DIRECTORY " DIRECTORY"},
        {NULL, NULL, LV_SYMBOL_ENVELOPE " ENVELOPE"},
        {NULL, NULL, LV_SYMBOL_SETTINGS " SETTINGS"},
        {NULL, NULL, LV_SYMBOL_POWER " POWER"},
    };
    size_t num = sizeof(applist) / sizeof(applist[0]);
    for (size_t i = 0; i < num; i++)
    {
        applist[i].btn = lv_btn_create(parent);
        lv_obj_add_style(applist[i].btn, &applist_style, 0);
        applist[i].label = lv_label_create(applist[i].btn);

        lv_obj_set_size(applist[i].btn, lv_pct(100), LV_SIZE_CONTENT);

        lv_label_set_text(applist[i].label, applist[i].text);
    }
}
