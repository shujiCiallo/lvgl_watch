#include "ui.h"
#include "../../lvgl/lvgl.h"
#include "screen/screen_manager.h"
#include "navigation/screen_navigator.h"
#include "style/app_styles.h"

/* UI 初始化入口:先初始化全局样式与导航,再创建主 tileview */
void ui_init(void)
{
    lv_group_t *default_group = lv_group_create();
    lv_indev_t *indev = lv_indev_get_next(NULL);
    if (indev) {
        lv_indev_set_group(indev, default_group);
    }
    styles_init();
    navigator_init();
    screen_tileview_create(lv_scr_act());
}
