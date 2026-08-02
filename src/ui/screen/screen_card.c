#include "screen_card.h"
#include "style/app_colors.h"
#include "style/app_styles.h"
#include "core/data_center.h"

static lv_obj_t *s_screen_card;

static void title_create(lv_obj_t *parent);
static void card_create(lv_obj_t *parent);

void screen_card_create(lv_obj_t *parent)
{
    s_screen_card = parent;
    lv_obj_set_style_pad_all(s_screen_card, 8, 0);
    lv_obj_set_flex_flow(s_screen_card, LV_FLEX_FLOW_COLUMN);
    // lv_obj_add_style(s_screen_card, &style_screen_bg, 0);

    lv_obj_t *title = lv_obj_create(parent);
    lv_obj_remove_style_all(title);
    lv_obj_set_size(title, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(title, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title, LV_FLEX_ALIGN_SPACE_BETWEEN,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    title_create(title);

    lv_obj_t *card_panel = lv_obj_create(s_screen_card);
    lv_obj_set_flex_flow(card_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_remove_style_all(card_panel);
    card_create(card_panel);
    lv_obj_set_size(card_panel, lv_pct(100), LV_SIZE_CONTENT);
}

static lv_style_t title_style;

static void title_create(lv_obj_t *parent)
{
    lv_style_init(&title_style);
    lv_style_set_text_font(&title_style, &lv_font_montserrat_24);

    lv_obj_add_style(parent, &title_style, 0);
    {
        lv_obj_t *time_label = lv_label_create(parent);
        lv_label_bind_text(time_label, &g_time_subject, "%s");

        lv_obj_t *date = lv_label_create(parent);
        lv_label_set_text(date, "date");
    }
}

static void card_create(lv_obj_t *parent)
{
    lv_obj_t *sport_card = lv_btn_create(parent);
    lv_obj_set_style_bg_color(sport_card, COLOR_SURFACE, 0);
    lv_obj_set_flex_flow(sport_card, LV_FLEX_FLOW_ROW);
    lv_coord_t scr_h = lv_disp_get_ver_res(NULL);
    lv_obj_set_size(sport_card, lv_pct(100), (scr_h*45/100));
    {
        lv_obj_t *arc = lv_arc_create(sport_card);
        lv_obj_set_flex_grow(arc, 1);
        lv_arc_set_range(arc, 0, 100);
        lv_arc_set_value(arc, 60);
        lv_arc_set_bg_angles(arc, 180, 0);
        lv_obj_set_size(arc, lv_pct(50), lv_pct(100));

        lv_obj_t *info = lv_obj_create(sport_card);
        lv_obj_set_flex_grow(info, 1);
        lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_text_color(info, lv_color_black(), 0);
        lv_obj_set_style_text_font(info, &lv_font_montserrat_24, 0);

        lv_obj_t *label1,*label2,*label3;
        lv_obj_t *label[] = {label1, label2, label3};

        size_t num = sizeof(label)/sizeof(label[0]);
        for (size_t i = 0; i < num; i++) {
            label[i] = lv_label_create(info);
            lv_obj_set_flex_grow(label[i],1);
            lv_label_set_text(label[i],"Ciallo");
        }
    }
}
