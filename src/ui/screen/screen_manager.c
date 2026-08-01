#include "screen_manager.h"
#include "screen_app.h"
#include "screen_main.h"
#include "screen_tools.h"
#include "screen/screen_card.h"
#include "event/event.h"

lv_obj_t* g_tileview;
int g_tile_index;

static void tileview_event_cb(lv_event_t *e);

void screen_tileview_create(lv_obj_t *parent)
{
    g_tileview = lv_tileview_create(parent);
    lv_obj_set_size(g_tileview, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(g_tileview, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *home_panel = lv_tileview_add_tile(g_tileview, 1, 1, LV_DIR_ALL);
    lv_obj_t *tool_panel = lv_tileview_add_tile(g_tileview, 0, 1, LV_DIR_RIGHT);
    lv_obj_t *card_panel = lv_tileview_add_tile(g_tileview, 2, 1, LV_DIR_LEFT);
    lv_obj_t *msg_panel = lv_tileview_add_tile(g_tileview, 1, 0, LV_DIR_BOTTOM);
    lv_obj_t *app_panel = lv_tileview_add_tile(g_tileview, 1, 2, LV_DIR_TOP);

    screen_tools_create(tool_panel);
    screen_app_create(app_panel);
    screen_card_create(card_panel);
    screen_main_create(home_panel);
    lv_tileview_set_tile(g_tileview, home_panel, LV_ANIM_OFF);

}

static void tileview_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_GESTURE) {

    }
}
