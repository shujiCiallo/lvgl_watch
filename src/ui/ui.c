#include "ui.h"
#include "../../lvgl/lvgl.h"
#include "screen/screen_manager.h"
#include "style/app_styles.h"

/* UI 初始化入口:先初始化全局样式,再创建主 tileview */
void ui_init(void)
{
    styles_init();
    screen_tileview_create(lv_scr_act());
}
