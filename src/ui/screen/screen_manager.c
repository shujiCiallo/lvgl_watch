#include "screen_manager.h"
#include "screen_main.h"
#include "screen_tools.h"
#include "event/event.h"

lv_obj_t* g_tileview;
int g_tile_index;

void screen_tileview_create(lv_obj_t *parent)
{
    lv_obj_t *g_tileview = lv_tileview_create(parent);
    lv_obj_set_size(g_tileview, lv_pct(100), lv_pct(100));
    lv_obj_set_scrollbar_mode(g_tileview, LV_SCROLLBAR_MODE_OFF);

    lv_obj_t *home_panel = lv_tileview_add_tile(g_tileview, 1, 1, LV_DIR_ALL);
    lv_obj_t *tool_panel = lv_tileview_add_tile(g_tileview, 0, 1, LV_DIR_RIGHT);
    lv_obj_t *card_panel = lv_tileview_add_tile(g_tileview, 2, 1, LV_DIR_LEFT);
    lv_obj_t *msg_panel = lv_tileview_add_tile(g_tileview, 1, 2, LV_DIR_TOP);
    lv_obj_t *app_panel = lv_tileview_add_tile(g_tileview, 1, 0, LV_DIR_BOTTOM);

    screen_main_create(home_panel);
    lv_tileview_set_tile(g_tileview, home_panel, LV_ANIM_OFF);

    screen_tools_create(tool_panel);
}

static void tileview_event_cb(lv_event_t *e)
{
    lv_obj_t *tv = lv_event_get_target(e);
    lv_obj_t *act = lv_tileview_get_tile_act(tv);
}
