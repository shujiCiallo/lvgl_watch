#include "HRmonitor.h"
#include "style/app_colors.h"
#include "screen/screen_card.h"

static HR_title_create(lv_obj_t *parent);
static HR_table_create(lv_obj_t *parent);
static void *HR_btn_click_cb(lv_event_cb_t *e);

lv_obj_t *HRmonitor_card_create(screen_card_t *self, lv_obj_t *parent)
{
    lv_obj_t *HR_card = lv_obj_create(parent);
    lv_obj_set_style_bg_color(HR_card, COLOR_SURFACE, 0);
    lv_obj_set_flex_flow(HR_card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(HR_card, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_coord_t scr_h = lv_disp_get_ver_res(NULL);
    lv_obj_set_size(HR_card, lv_pct(100), (scr_h * 45 / 100));
    lv_obj_add_event_cb(HR_card, HR_btn_click_cb,
        LV_EVENT_CLICKED, self);

   HR_title_create(HR_card);
    
   HR_table_create(HR_card);

}

static HR_title_create(lv_obj_t *parent)
{
    lv_obj_t *HR_title = lv_obj_create(parent);
    lv_obj_remove_style_all(HR_title);
    lv_obj_set_width(HR_title, lv_pct(100));
    lv_obj_set_flex_grow(HR_title, 2);
    lv_obj_set_flex_flow(HR_title, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(HR_title, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t *HR_label = lv_label_create(HR_title);
    lv_obj_set_flex_grow(HR_label, 2);
    lv_label_set_text(HR_label, "heart rate");

    lv_obj_t *HR_data_label = lv_label_create(HR_title);
    lv_obj_set_flex_grow(HR_data_label, 3);
    lv_label_set_text(HR_data_label, "data  data");
}

static HR_table_create(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    // lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_grow(obj, 3);
    

}
static void *HR_btn_click_cb(lv_event_cb_t *e)
{

}