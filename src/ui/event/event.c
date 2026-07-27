#include "lvgl/lvgl.h"
#include "event.h"

time_t current_time;
struct tm *local_time;
char time_buf[24];

void time_cb(lv_timer_t *t)
{
    lv_obj_t *label = lv_timer_get_user_data(t);

    current_time = time(NULL);
    local_time = localtime(&current_time);
    strftime(time_buf, sizeof(time_buf), "%H:%M", local_time);
    lv_label_set_text(label, time_buf);

}
