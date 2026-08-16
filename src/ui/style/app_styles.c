#include "app_colors.h"
#include "app_styles.h"

/* 全局共享样式:各屏通过 extern 引用 */
lv_style_t style_screen_bg;
lv_style_t info_style;

static void app_styles_init(void);
static void info_style_init(void);

/* 全局样式初始化入口,由 ui_init 调用 */
void styles_init(void)
{
    app_styles_init();
    info_style_init();
}

/* 屏幕背景样式:浅灰背景 + 内边距 */
static void app_styles_init(void)
{
    lv_style_init(&style_screen_bg);
    lv_style_set_bg_color(&style_screen_bg, COLOR_BACKGROUND);
    lv_style_set_pad_all(&style_screen_bg, 16);
}

/* 状态图标样式:信息面板图标文本使用 */
static void info_style_init(void)
{
    lv_style_init(&info_style);
    lv_style_set_text_font(&info_style, &lv_font_montserrat_16);
}
