#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "screen_app.h"
#include "style/app_styles.h"
#include "style/app_colors.h"
#include "screen/screen_manager.h"

/* 应用列表构建函数,只在模块内部使用 */
static void applist_create(lv_obj_t *parent);
static void power_btn_click_cb(lv_event_t *e);

/* 创建应用列表屏幕:纵向排列的应用按钮网格 */
void screen_app_create(screen_app_t *self, lv_obj_t *parent)
{
    self->root = parent;   /* 保存根对象到结构体 */
    lv_obj_add_style(self->root, &style_screen_bg, 0);
    lv_obj_set_flex_flow(self->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(self->root, 0, 0);

    applist_create(self->root);
}

static lv_style_t applist_style;   /* 应用按钮文本样式,模块级单例 */

/* 点击 POWER:关闭模拟器 */
static void power_btn_click_cb(lv_event_t *e)
{
    (void)e;
    exit(0);
}

/* 应用列表:用 Button_t 配置数组批量创建 10 个应用按钮 */
static void applist_create(lv_obj_t *parent)
{
    lv_style_init(&applist_style);
    lv_style_set_text_font(&applist_style, &lv_font_montserrat_24);
    lv_style_set_bg_color(&applist_style, COLOR_BACKGROUND);
    lv_style_set_text_color(&applist_style, lv_color_black());
    lv_style_set_margin_all(&applist_style, 8);
    // lv_style_set_bg_opa(&applist_style, 0);
    // lv_style_set_border_width(&applist_style, 0);

    // Button_t applist[] = {
    //     {NULL, NULL, LV_SYMBOL_VOLUME_MID " VOLUME"},
    //     {NULL, NULL, LV_SYMBOL_VIDEO " VIDEO"},
    //     {NULL, NULL, LV_SYMBOL_IMAGE " IMAGE"},
    //     {NULL, NULL, LV_SYMBOL_CALL " CALL"},
    //     {NULL, NULL, LV_SYMBOL_WIFI " WIFI"},
    //     {NULL, NULL, LV_SYMBOL_BLUETOOTH " BLUETOOTH"},
    //     {NULL, NULL, LV_SYMBOL_DIRECTORY " DIRECTORY"},
    //     {NULL, NULL, LV_SYMBOL_ENVELOPE " ENVELOPE"},
    //     {NULL, NULL, LV_SYMBOL_SETTINGS " SETTINGS"},
    //     {NULL, NULL, LV_SYMBOL_POWER " POWER"},
    // };
    Button_t applist[] = {
        {NULL, NULL, " VOLUME"},
        {NULL, NULL, " VIDEO"},
        {NULL, NULL, " IMAGE"},
        {NULL, NULL, " CALL"},
        {NULL, NULL, " WIFI"},
        {NULL, NULL, " BLUETOOTH"},
        {NULL, NULL, " DIRECTORY"},
        {NULL, NULL, " ENVELOPE"},
        {NULL, NULL, " SETTINGS"},
        {NULL, NULL, " POWER"},
    };

    char *app_text[] = {
        LV_SYMBOL_VOLUME_MID,
        LV_SYMBOL_VIDEO ,
        LV_SYMBOL_IMAGE ,
        LV_SYMBOL_CALL ,
        LV_SYMBOL_WIFI ,
        LV_SYMBOL_BLUETOOTH ,
        LV_SYMBOL_DIRECTORY ,
        LV_SYMBOL_ENVELOPE ,
        LV_SYMBOL_SETTINGS ,
        LV_SYMBOL_POWER ,
    };

    size_t num = sizeof(applist) / sizeof(applist[0]);
    for (size_t i = 0; i < num; i++)
    {
        // applist[i].btn = lv_obj_create(parent);
        // // lv_obj_remove_style_all(applist[i].btn);
        // lv_obj_add_style(applist[i].btn, &applist_style, 0);
        // lv_obj_set_size(applist[i].btn, lv_pct(100), LV_SIZE_CONTENT);

        // applist[i].label = lv_label_create(applist[i].btn);
        applist[i].label = lv_obj_create(parent);
        lv_obj_set_flex_flow(applist[i].label, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(applist[i].label, LV_FLEX_ALIGN_START, 
            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_set_size(applist[i].label, lv_pct(100), LV_SIZE_CONTENT);
        // lv_obj_add_style(applist[i].label, &applist_style, 0);

        lv_obj_t *label_symbol = lv_label_create(applist[i].label);
        lv_label_set_text(label_symbol, app_text[i]);
        lv_obj_set_style_text_font(label_symbol, &lv_font_montserrat_36, 0);
        lv_obj_set_size(label_symbol, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        
        lv_obj_t *label_text = lv_label_create(applist[i].label);
        lv_label_set_text(label_text, applist[i].text);
        lv_obj_set_style_text_font(label_text, &lv_font_montserrat_24, 0);
        lv_obj_set_size(label_text, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        

        /* 点击 POWER 按钮关闭程序 */
        if (strstr(applist[i].text, "POWER") != NULL) {
            lv_obj_add_event_cb(applist[i].btn, power_btn_click_cb, LV_EVENT_CLICKED, NULL);
        }
    }
}
