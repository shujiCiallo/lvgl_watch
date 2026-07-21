#include "screen_main.h"
#include "lvgl/lvgl.h"
#include "event/event.h"
#include "style/app_styles.h"
#include <time.h>

void screen_main_create(void)
{
    lv_obj_t *base_scr = lv_scr_act();
    lv_obj_add_style(base_scr, &style_screen_bg, 0);

    lv_obj_t *home_panel = lv_obj_create(base_scr);
    lv_obj_set_flex_flow(home_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(home_panel, lv_pct(100), lv_pct(100));

    lv_obj_t *time_panel = lv_obj_create(home_panel);
    lv_obj_set_align(time_panel, LV_ALIGN_TOP_MID);
    lv_obj_set_size(time_panel, lv_pct(100), lv_pct(40));

    lv_obj_t *volume_panel = lv_obj_create(home_panel);
    lv_obj_set_size(volume_panel, lv_pct(100), lv_pct(15));
    lv_obj_t *volume_slider = lv_slider_create(volume_panel);
    lv_obj_center(volume_slider);

    lv_obj_t *info_panel = lv_obj_create(home_panel);
    lv_obj_set_size(info_panel, lv_pct(100), lv_pct(35));
    lv_obj_set_flex_flow(info_panel, LV_FLEX_FLOW_ROW);


    {
        lv_obj_t *temp = lv_obj_create(info_panel);
        lv_obj_set_flex_grow(temp, 1);
        lv_obj_set_height(temp, lv_pct(100));

        lv_obj_t *compass = lv_obj_create(info_panel);
        lv_obj_set_flex_grow(compass, 1);
        lv_obj_set_height(compass, lv_pct(100));

        lv_obj_t *BAT = lv_obj_create(info_panel);
        lv_obj_set_flex_grow(BAT, 1);
        lv_obj_set_height(BAT, lv_pct(100));

    }

    lv_obj_t *time_label = lv_label_create(time_panel);
    // lv_obj_set_size(time_label, lv_pct(100), lv_pct(100));
    lv_obj_center(time_label);
    lv_obj_set_style_bg_opa(time_label, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(time_label, lv_color_hex(0x134f234), 0);
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
    // lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0);

    lv_timer_create(time_cb, 1000, time_label);


}
