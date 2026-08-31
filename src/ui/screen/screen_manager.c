#include "screen_manager.h"
#include "screen_app.h"
#include "screen_main.h"
#include "screen_tools.h"
#include "screen/screen_card.h"
#include "event/event.h"

lv_obj_t *g_tileview;
int g_tile_index = 2;   /* 预留:当前所在 tile 的索引,暂未使用 */

/* 各屏幕运行时状态(模块内单例),传给对应屏的 create 接口 */
static screen_main_t     s_main;
static screen_tools_t    s_tools;
static screen_card_t     s_sport_card;
static screen_app_t      s_app;
static lv_obj_t         *s_msg_tile;    /* 消息面板 tile,尚无独立屏结构体 */

/* 运动卡片屏幕单例访问:event 与各界面点击共用 */
screen_card_t *sport_card_inst(void)
{
    return &s_sport_card;
}

static void tileview_event_cb(lv_event_t *e);
static void tileview_gesture_bubble_all(lv_obj_t *obj);

/* 创建主 tileview:5 个面板,靠左右上下滑动切换 */
void screen_tileview_create(lv_obj_t *parent)
{
    g_tileview = lv_tileview_create(parent);
    lv_obj_set_size(g_tileview, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(g_tileview, LV_SCROLLBAR_MODE_OFF);

    /* 定义 5 个面板的位置坐标 */
    lv_obj_t *home_panel = lv_tileview_add_tile(g_tileview, 1, 1, LV_DIR_ALL);
    lv_obj_t *tool_panel = lv_tileview_add_tile(g_tileview, 0, 1, LV_DIR_RIGHT);
    lv_obj_t *card_panel = lv_tileview_add_tile(g_tileview, 2, 1, LV_DIR_LEFT);
    lv_obj_t *msg_panel  = lv_tileview_add_tile(g_tileview, 1, 0, LV_DIR_BOTTOM);
    lv_obj_t *app_panel  = lv_tileview_add_tile(g_tileview, 1, 2, LV_DIR_TOP);
    s_msg_tile = msg_panel;

    /* 各屏状态写入结构体实例,不再依赖屏内静态全局 */
    screen_tools_create(&s_tools, tool_panel);
    screen_app_create(&s_app, app_panel);
    screen_card_create(&s_sport_card, card_panel);
    screen_main_create(&s_main, home_panel);
    lv_tileview_set_tile(g_tileview, home_panel, LV_ANIM_OFF);

    // lv_obj_add_event_cb(g_tileview, tileview_event_cb,
    //     LV_EVENT_GESTURE, NULL);
    // tileview_gesture_bubble_all(g_tileview);
}

// /* 让 tileview 所有子孙手势冒泡到 tileview,tileview 自身不冒泡以便命中根回调 */
// static void tileview_gesture_bubble_all(lv_obj_t *obj)
// {
//     uint32_t cnt = lv_obj_get_child_count(obj);
//     for (uint32_t i = 0; i < cnt; i++) {
//         lv_obj_t *child = lv_obj_get_child(obj, i);
//         lv_obj_set_gesture_bubble(child, true);
//         tileview_gesture_bubble_all(child);
//     }
// }

// /* 预留:手势切换回调,暂未启用 */
// static void tileview_event_cb(lv_event_t *e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     if (code == LV_EVENT_GESTURE) {
//         lv_indev_t *indev = lv_indev_get_act();
//         if (!indev) { return; }

//         lv_dir_t dir = lv_indev_get_gesture_dir(indev);

//         if (dir == LV_DIR_RIGHT) {
//             lv_obj_t *arc_tile = lv_tileview_get_tile_act(g_tileview);

//             if (arc_tile == s_app.root || arc_tile == s_msg_tile) {
//                 lv_tileview_set_tile(g_tileview, s_main.root, LV_ANIM_ON);
//             }
//         }

//     }
// }
