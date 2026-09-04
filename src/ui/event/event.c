#include "lvgl/lvgl.h"
#include "event/music.h"
#include "event.h"
#include "event/sport_panel.h"
#include "event/compass_panel.h"
#include "navigation/screen_navigator.h"

/* 点击打开运动详情页 */
void sport_btn_click_cb(lv_event_t *e)
{
    (void)e;
    static sport_panel_t p;
    sport_panel_create(&p);
    navigator_push(p.root);
}

/* 点击打开指南针详情页 */
void compass_btn_click_cb(lv_event_t *e)
{
    (void)e;
    static compass_panel_t p;
    compass_panel_create(&p);
    navigator_push(p.root);
}

/* 点击打开音乐详情页 */
void music_btn_click_cb(lv_event_t *e)
{
    (void)e;
    static music_panel_t p;
    music_panel_create(&p);
    navigator_push(p.root);
}