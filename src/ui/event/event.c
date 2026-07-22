#include "lvgl/lvgl.h"
#include "event.h"

time_t current_time;
struct tm *local_time;
char time_buf[24];

void slider_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *slider_label = lv_event_get_user_data(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_label_set_text_fmt(slider_label, "%d", lv_slider_get_value(slider));
    }
}

void btn_event_cb(lv_event_t *e)
{
    int *btn_id = lv_event_get_user_data(e);
    lv_event_code_t code  = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *btn_lab = lv_obj_get_child(btn, 0);

    switch (*btn_id)
    {
    case 0:
        if (code == LV_EVENT_PRESSING) {

        } else if(code == LV_EVENT_RELEASED) {

        }
        break;

    default:
        break;
    }

}

lv_obj_t *setting_screen_create(void)
{
    lv_obj_t *setting = lv_obj_create(NULL);
    lv_obj_set_size(setting, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(setting,
        lv_color_hex(0x0066cc), LV_PART_MAIN);

    lv_obj_t *label = lv_label_create(setting);
    lv_obj_set_align(label, LV_ALIGN_TOP_MID);
    lv_label_set_text(label, "setting");

    return setting;
}

void time_cb(lv_timer_t *t)
{
    lv_obj_t *label = lv_timer_get_user_data(t);

    current_time = time(NULL);
    local_time = localtime(&current_time);
    strftime(time_buf, sizeof(time_buf), "%H:%M", local_time);
    lv_label_set_text(label, time_buf);

}
