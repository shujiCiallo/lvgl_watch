#include "lvgl/lvgl.h"
#include "event.h"
#include "screen/card/sport_card.h"
#include "event/compass_panel.h"
#include "navigation/screen_navigator.h"

/* 点击打开运动详情页:多界面公用,user_data 为 screen_card_t* */
void sport_btn_click_cb(lv_event_t *e)
{
    screen_card_t *self = lv_event_get_user_data(e);
    lv_obj_t *new = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(new, 8, 0);
    lv_obj_set_size(new, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(new, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollable(new, false);

    sport_panel_creata(self, new);
    navigator_push(new);
}

/* 点击打开指南针详情页 */
void compass_btn_click_cb(lv_event_t *e)
{
    (void)e;
    lv_obj_t *new = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(new, 8, 0);
    lv_obj_set_size(new, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(new, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollable(new, false);

    compass_panel_create(new);
    navigator_push(new);
}
