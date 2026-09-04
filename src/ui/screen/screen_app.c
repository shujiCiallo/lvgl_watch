#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "screen_app.h"
#include "style/app_styles.h"
#include "style/app_colors.h"
#include "screen/screen_manager.h"
#include "event/setting_page.h"
#include "event/music.h"
#include "event/event.h"

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
    lv_sdl_quit();
}

/* 应用列表:用 Button_t 配置数组批量创建 10 个应用按钮 */
static void applist_create(lv_obj_t *parent)
{
    lv_style_init(&applist_style);
    lv_style_set_text_font(&applist_style, &lv_font_montserrat_24);
    lv_style_set_bg_color(&applist_style, COLOR_BACKGROUND);
    lv_style_set_text_color(&applist_style, lv_color_black());
    lv_style_set_margin_all(&applist_style, 8);

    Button_t applist[] = {
        {NULL, NULL, " VOLUME"},
        {NULL, NULL, " MUSIC"},
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

    lv_event_cb_t app_cb[] = {
        NULL,
        music_btn_click_cb,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        setting_btn_click_cb,
        power_btn_click_cb       
    };

    size_t num = sizeof(applist) / sizeof(applist[0]);
    for (size_t i = 0; i < num; i++)
    {
        applist[i].label = lv_obj_create(parent);
        lv_obj_set_flex_flow(applist[i].label, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(applist[i].label, LV_FLEX_ALIGN_START, 
            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_set_size(applist[i].label, lv_pct(100), lv_pct(15));
        lv_obj_set_style_bg_opa(applist[i].label, 0, 0);
        lv_obj_set_style_border_width(applist[i].label, 0, 0);
        lv_obj_set_scrollable(applist[i].label, false);
        lv_obj_set_checkable(applist[i].label, true);

        lv_obj_t *label_symbol = lv_label_create(applist[i].label);
        lv_label_set_text(label_symbol, app_text[i]);
        lv_obj_set_style_text_font(label_symbol, &lv_font_montserrat_36, 0);
        lv_obj_set_size(label_symbol, lv_pct(20), LV_SIZE_CONTENT);
        
        lv_obj_t *label_text = lv_label_create(applist[i].label);
        lv_label_set_text(label_text, applist[i].text);
        lv_obj_set_style_text_font(label_text, &lv_font_montserrat_24, 0);
        lv_obj_set_size(label_text, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
        
        if (app_cb[i] != NULL) {
            lv_obj_add_event_cb(applist[i].label, app_cb[i],
                LV_EVENT_CLICKED, NULL);
        }

    }
}

