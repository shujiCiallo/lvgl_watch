#ifndef __EVENT_H
#define __EVENT_H

void slider_event_cb(lv_event_t *e);
void btn_event_cb(lv_event_t *e);
lv_obj_t *setting_screen_create(void);
void time_cb(lv_timer_t *time);

#endif
