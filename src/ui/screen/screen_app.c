#include "lvgl.h"
#include "style/app_styles.h"
#include "style/app_colors.h"
#include "screen/screen_manager.h"

static lv_obj_t *s_screen_app = NULL;

static void applist_create(lv_obj_t *parent);

void screen_app_create(lv_obj_t *parent)
{
    s_screen_app = parent;
    lv_obj_add_style(s_screen_app, &style_screen_bg, 0);
    lv_obj_set_flex_flow(s_screen_app, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_column(s_screen_app, 5, 0);
    lv_obj_set_style_pad_row(s_screen_app, 5, 0);

    applist_create(s_screen_app);
}

static lv_style_t applist_style;

static void applist_create(lv_obj_t *parent)
{

    lv_style_init(&applist_style);
    lv_style_set_text_font(&applist_style, &lv_font_montserrat_24);
    lv_style_set_bg_color(&applist_style, COLOR_BACKGROUND);
    lv_style_set_text_color(&applist_style, lv_color_black());

    Button_t applist[] = {
        {NULL,NULL,LV_SYMBOL_VOLUME_MID " VOLUME"},
        {NULL,NULL,LV_SYMBOL_VIDEO " VIDEO"},
        {NULL,NULL,LV_SYMBOL_IMAGE " IMAGE"},
        {NULL,NULL,LV_SYMBOL_CALL " CALL"},
        {NULL,NULL,LV_SYMBOL_WIFI " WIFI"},
        {NULL,NULL,LV_SYMBOL_BLUETOOTH " BLUETOOTH"},
        {NULL,NULL,LV_SYMBOL_DIRECTORY " DIRECTORY"},
        {NULL,NULL,LV_SYMBOL_ENVELOPE " ENVELOPE"},
        {NULL,NULL,LV_SYMBOL_SETTINGS " SETTINGS"},
        {NULL,NULL,LV_SYMBOL_POWER " POWER"},
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
