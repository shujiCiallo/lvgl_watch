#include "app_colors.h"
#include "app_styles.h"

lv_style_t style_screen_bg;
lv_style_t info_style;

static void app_styles_init(void);
static void info_style_init(void);

void styles_init(void)
{
    app_styles_init();
    info_style_init();
}

static void app_styles_init(void)
{
    lv_style_init(&style_screen_bg);
    lv_style_set_bg_color(&style_screen_bg, COLOR_BACKGROUND);
    lv_style_set_pad_all(&style_screen_bg, 16);
}

static void info_style_init(void)
{
    lv_style_init(&info_style);
    lv_style_set_text_font(&info_style, &lv_font_montserrat_36);
    // lv_style_set_bg_color(&s_info_style, lv_color_hex(0x1234f234));
    // lv_style_set_bg_opa(&s_info_style, LV_OPA_COVER);
}
