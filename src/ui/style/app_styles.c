#include "app_colors.h"
#include "app_styles.h"

lv_style_t style_screen_bg;

void app_styles_init(void)
{
    lv_style_init(&style_screen_bg);
    lv_style_set_bg_color(&style_screen_bg, COLOR_BACKGROUND);
    lv_style_set_pad_all(&style_screen_bg, 16);
}