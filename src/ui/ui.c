#include "ui.h"
#include "../../lvgl/lvgl.h"
#include "screen/screen_main.h"
#include "style/app_styles.h"

void ui_init(void)
{
    app_styles_init();
    screen_main_create();
}
