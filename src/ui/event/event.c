#include "lvgl/lvgl.h"
#include "event.h"
#include "screen/card/sport_card.h"

/* sport 详情页右滑返回与手势冒泡(模块内使用) */
static void sport_panel_gesture_cb(lv_event_t *e);
static void sport_gesture_bubble_all(lv_obj_t *obj);

/* compass 详情页右滑返回与 UI 构建(模块内使用) */
static void compass_panel_gesture_cb(lv_event_t *e);
static void compass_panel_create(screen_card_t *self, lv_obj_t *parent);

/* 点击打开运动详情页:多界面公用,user_data 为 screen_card_t* */
void sport_btn_click_cb(lv_event_t *e)
{
    screen_card_t *self = lv_event_get_user_data(e);
    lv_obj_t *old_scr = lv_scr_act();
    lv_obj_add_flag(old_scr, LV_OBJ_FLAG_HIDDEN);

    if (self->sport_scr == NULL) {
        /* 首次进入:创建详情页并绑定右滑手势 */
        lv_obj_t *new = lv_obj_create(NULL);
        lv_obj_set_style_pad_all(new, 8, 0);
        lv_obj_set_size(new, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(new, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scrollable(new, false);

        sport_panel_creata(self, new);

        self->sport_scr = new;
        lv_obj_add_event_cb(new, sport_panel_gesture_cb, LV_EVENT_GESTURE, self);
        sport_gesture_bubble_all(new);
    }
    else {
        /* 复用详情页:取消隐藏即可 */
        lv_obj_remove_flag(self->sport_scr, LV_OBJ_FLAG_HIDDEN);
    }

    self->main_scr = old_scr;
    lv_scr_load(self->sport_scr);
}

/* 详情页右滑返回:隐藏详情页并恢复原屏(不删除,复用) */
static void sport_panel_gesture_cb(lv_event_t *e)
{
    screen_card_t *self = lv_event_get_user_data(e);
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    if (dir != LV_DIR_RIGHT) return;

    lv_obj_add_flag(self->sport_scr, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(self->main_scr, LV_OBJ_FLAG_HIDDEN);
    lv_scr_load(self->main_scr);
}

/* 让 obj 的所有子孙手势冒泡到 obj,obj 自身不冒泡以便命中根回调 */
static void sport_gesture_bubble_all(lv_obj_t *obj)
{
    uint32_t cnt = lv_obj_get_child_count(obj);
    for (uint32_t i = 0; i < cnt; i++) {
        lv_obj_t *child = lv_obj_get_child(obj, i);
        lv_obj_set_gesture_bubble(child, true);
        sport_gesture_bubble_all(child);
    }
}

void compass_btn_click_cb(lv_event_t *e)
{
    screen_card_t *self = lv_event_get_user_data(e);
    lv_obj_t *old_scr = lv_scr_act();
    lv_obj_add_flag(old_scr, LV_OBJ_FLAG_HIDDEN);

    if (self->compass_scr == NULL) {
        /* 首次进入:创建详情页并绑定右滑手势 */
        lv_obj_t *new = lv_obj_create(NULL);
        lv_obj_set_style_pad_all(new, 8, 0);
        lv_obj_set_size(new, lv_pct(100), lv_pct(100));
        lv_obj_set_flex_flow(new, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_scrollable(new, false);

        compass_panel_create(self, new);

        self->compass_scr = new;
        lv_obj_add_event_cb(new, compass_panel_gesture_cb, LV_EVENT_GESTURE, self);
        sport_gesture_bubble_all(new);
    }
    else {
        /* 复用详情页:取消隐藏即可 */
        lv_obj_remove_flag(self->compass_scr, LV_OBJ_FLAG_HIDDEN);
    }

    self->main_scr = old_scr;
    lv_scr_load(self->compass_scr);
}

/* 指南针详情页右滑返回:隐藏详情页并恢复原屏(不删除,复用) */
static void compass_panel_gesture_cb(lv_event_t *e)
{
    screen_card_t *self = lv_event_get_user_data(e);
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    if (dir != LV_DIR_RIGHT) return;

    lv_obj_add_flag(self->compass_scr, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag(self->main_scr, LV_OBJ_FLAG_HIDDEN);
    lv_scr_load(self->main_scr);
}

/* 指南针详情页 UI(后续手动实现) */
static void compass_panel_create(screen_card_t *scr, lv_obj_t *parent)
{

}