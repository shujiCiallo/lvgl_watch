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
    {
        lv_obj_t *time_label = lv_label_create(time_panel);
        lv_obj_center(time_label);
        lv_obj_set_style_bg_opa(time_label, LV_OPA_COVER, 0);
        lv_obj_set_style_text_font(time_label, &lv_font_montserrat_48, 0);
        current_time = time(NULL);
        local_time = localtime(&current_time);
        strftime(time_buf, sizeof(time_buf), "%H:%M", local_time);
        lv_label_set_text(time_label, time_buf);

        lv_timer_create(time_cb, 1000, time_label);
    }

    lv_obj_t *volume_panel = lv_obj_create(home_panel);
    lv_obj_set_size(volume_panel, lv_pct(100), lv_pct(15));
    lv_obj_t *volume_slider = lv_slider_create(volume_panel);
    lv_obj_center(volume_slider);

    lv_obj_t *info_panel = lv_obj_create(home_panel);
    lv_obj_set_size(info_panel, lv_pct(100), lv_pct(35));
    lv_obj_set_flex_flow(info_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(info_panel,
                      LV_FLEX_ALIGN_START,
                      LV_FLEX_ALIGN_CENTER,
                      LV_FLEX_ALIGN_START);
    {
        static lv_style_t info_style;
        lv_style_init(&info_style);
        lv_style_set_text_font(&info_style, &lv_font_montserrat_36);
        // lv_style_set_bg_color(&info_style, lv_color_hex(0x1234f234));
        // lv_style_set_bg_opa(&info_style, LV_OPA_COVER);

        lv_obj_t *temp = lv_obj_create(info_panel);
        lv_obj_set_flex_grow(temp, 1);
        lv_obj_set_height(temp, lv_pct(100));
        lv_obj_t *sim_label = lv_label_create(temp);
        lv_label_set_text(sim_label, LV_SYMBOL_SD_CARD);
        lv_obj_add_style(sim_label, &info_style, 0);
        lv_obj_center(sim_label);


        lv_obj_t *compass = lv_obj_create(info_panel);
        lv_obj_set_flex_grow(compass, 1);
        lv_obj_set_height(compass, lv_pct(100));
        lv_obj_t *compass_label = lv_label_create(compass);
        lv_label_set_text(compass_label, LV_SYMBOL_GPS);
        lv_obj_add_style(compass_label, &info_style, 0);
        lv_obj_center(compass_label);

        lv_obj_t *BAT = lv_obj_create(info_panel);
        lv_obj_set_flex_grow(BAT, 1);
        lv_obj_set_height(BAT, lv_pct(100));
        lv_obj_t *BAT_label = lv_label_create(BAT);
        lv_label_set_text(BAT_label, LV_SYMBOL_BATTERY_1);
        lv_obj_add_style(BAT_label, &info_style, 0);
        lv_obj_center(BAT_label);
    }

}
