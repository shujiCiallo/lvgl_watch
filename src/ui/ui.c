#include "ui.h"
#include "../../lvgl/lvgl.h"
#include "screen/screen_manager.h"
#include "style/app_styles.h"

void ui_init(void)
{
    styles_init();
    screen_tileview_create(lv_scr_act());
}
