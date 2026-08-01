#include "screen_card.h"
#include "style/app_styles.h"
#include "core/data_center.h"

static lv_obj_t *s_screen_card;

static void title_create(lv_obj_t *parent);

void screen_card_create(lv_obj_t *parent)
{
    s_screen_card = parent;
    lv_obj_set_style_pad_all(s_screen_card, 8, 0);
    // lv_obj_add_style(s_screen_card, &style_screen_bg, 0);


    title_create(s_screen_card);

}

static lv_style_t title_style;

static void title_create(lv_obj_t *parent)
{
    lv_style_init(&title_style);
    lv_style_set_text_font(&title_style, &lv_font_montserrat_24);

    lv_obj_t *time_label = lv_label_create(parent);
    lv_obj_add_style(time_label, &title_style, 0);
    lv_label_bind_text(time_label, &g_time_subject, "%s");
    lv_obj_set_align(time_label, LV_ALIGN_TOP_LEFT);

    lv_obj_t *date = lv_label_create(parent);
    lv_obj_add_style(date, &title_style, 0);
    lv_label_set_text(date, "date");
    lv_obj_set_align(date, LV_ALIGN_TOP_RIGHT);
}
