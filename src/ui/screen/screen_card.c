#include "screen_card.h"
#include "style/app_colors.h"
#include "style/app_styles.h"
#include "core/data_center.h"

static lv_obj_t *s_screen_card;

static void title_create(lv_obj_t *parent);
static void card_create(lv_obj_t *parent);
static lv_obj_t *artivity_rings_create(lv_obj_t *parent);
static lv_obj_t *sport_card_create(lv_obj_t* parent);
static lv_obj_t *label_data_create(lv_obj_t *parent);

typedef struct label_data {
        lv_obj_t *parent;
        lv_obj_t *line1;
        lv_obj_t *line2;
}label_data_t;

void screen_card_create(lv_obj_t *parent)
{
    s_screen_card = parent;
    // lv_obj_add_style(s_screen_card, &style_screen_bg, 0);
    lv_obj_set_style_pad_all(s_screen_card, 8, 0);
    lv_obj_set_flex_flow(s_screen_card, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *title = lv_obj_create(parent);
    lv_obj_remove_style_all(title);
    lv_obj_set_size(title, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(title, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title, LV_FLEX_ALIGN_SPACE_BETWEEN,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_flex_grow(title, 0);

    title_create(title);

    lv_obj_t *card_panel = lv_obj_create(s_screen_card);
    lv_obj_remove_style_all(card_panel);
    lv_obj_set_flex_flow(card_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card_panel, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_flex_grow(card_panel, 1);
    // lv_obj_set_height(card_panel, lv_pct(100) - lv_obj_get_height(title));
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
        // lv_label_set_text(date, "date");
        lv_label_bind_text(date, &g_date_subject, "%s");
    }
}

static void card_create(lv_obj_t *parent)
{
    sport_card_create(parent);
}

static lv_obj_t *sport_card_create(lv_obj_t* parent)
{
    lv_obj_t *sport_card = lv_btn_create(parent);
    lv_obj_set_style_bg_color(sport_card, COLOR_SURFACE, 0);
    lv_obj_set_flex_flow(sport_card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sport_card, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_coord_t scr_h = lv_disp_get_ver_res(NULL);
    // lv_coord_t scr_w = lv_disp_get_hor_res(NULL);
    lv_obj_set_size(sport_card, lv_pct(100), (scr_h*45/100));
    {
        lv_obj_t *meter = lv_obj_create(sport_card);
        lv_obj_clear_flag(meter, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_remove_style_all(meter);
        lv_obj_set_flex_grow(meter, 1);
        // lv_obj_set_width(meter, lv_pct(100));
        // lv_obj_set_height(meter, lv_pct(50));
        // lv_obj_center(meter);
        artivity_rings_create(meter);

        lv_obj_t *info = lv_obj_create(sport_card);
        // lv_obj_remove_style_all(info);
        lv_obj_set_flex_grow(info, 1);
        // lv_obj_set_height(info, lv_pct(50));
        lv_obj_set_width(info, lv_pct(100));
        lv_obj_set_flex_flow(info, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(info, LV_FLEX_ALIGN_SPACE_EVENLY,
            LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_text_color(info, lv_color_black(), 0);
        lv_obj_set_style_text_font(info, &lv_font_montserrat_18, 0);
        lv_obj_set_style_pad_all(info, 8, 0);

        label_data_create(info);
    }

}

static lv_obj_t *artivity_rings_create(lv_obj_t *parent)
{
    typedef struct artivity_rings
    {
        lv_obj_t *arc;
        int size;
        int width;
        uint32_t max_value;
        lv_color_t color;
        void *data;
    }artivity_rings_t;

    artivity_rings_t configs[] = {
        {NULL, 60, 6, 400, COLOR_ERROR},
        {NULL, 90, 6, 6000, COLOR_PRIMARY_DARK},
        {NULL, 120, 6, 1200, COLOR_SECONDARY}
    };

    for (size_t i = 0; i < 3; i++) {
        configs[i].arc = lv_arc_create(parent);

        lv_obj_clear_flag(configs[i].arc, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(configs[i].arc, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_set_size(configs[i].arc, configs[i].size, configs[i].size);
        // lv_obj_center(configs[i].arc);
        lv_obj_align(configs[i].arc, LV_ALIGN_BOTTOM_MID,
            0, configs[i].size/2);

        // lv_obj_set_style_arc_opa(configs[i].arc, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_opa(configs[i].arc, LV_OPA_TRANSP, LV_PART_KNOB);

        lv_obj_set_style_arc_color(configs[i].arc, configs[i].color,
            LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(configs[i].arc, configs[i].width,
            LV_PART_INDICATOR);

        lv_arc_set_bg_angles(configs[i].arc, 180, 0);
        lv_arc_set_max_value(configs[i].arc, configs[i].max_value);
        lv_arc_set_value(configs[i].arc, 326);
    }

}


static lv_obj_t *label_data_create(lv_obj_t *parent)
{
    label_data_t label[3] = {};
    size_t num = sizeof(label)/sizeof(label[0]);
    char *title[] = {"calorie", "steps","times"};
    lv_subject_t *data[] = {
        &g_calorie_subject,
        &g_steps_subject,
        &g_duration_subject};
    for (size_t i = 0; i < num; i++) {
        label[i].parent = lv_obj_create(parent);
        lv_obj_remove_style_all(label[i].parent);
        lv_obj_set_flex_grow(label[i].parent,1);
        lv_obj_set_height(label[i].parent, lv_pct(100));
        lv_obj_set_flex_flow(label[i].parent, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(label[i].parent, LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

        label[i].line1 = lv_label_create(label[i].parent);
        // lv_obj_set_style_bg_color(label[i].line1, COLOR_ERROR, 0);
        lv_obj_set_style_bg_opa(label[i].line1, LV_OPA_COVER, 0);
        lv_obj_set_flex_grow(label[i].line1, 1);
        lv_obj_set_height(label[i].line1, lv_pct(100));
        lv_obj_set_width(label[i].line1, lv_pct(100));
        lv_obj_set_style_text_align(label[i].line1, LV_TEXT_ALIGN_CENTER, 0);
        // lv_obj_center(label[i].line1);
        lv_label_set_text(label[i].line1, title[i]);

        label[i].line2 = lv_label_create(label[i].parent);
        // lv_obj_set_style_bg_color(label[i].line2, COLOR_ERROR, 0);
        lv_obj_set_style_bg_opa(label[i].line2, LV_OPA_COVER, 0);
        lv_obj_set_flex_grow(label[i].line2, 1);
        lv_obj_set_height(label[i].line2, lv_pct(100));
        lv_obj_set_width(label[i].line2, lv_pct(100));
        lv_obj_set_style_text_align(label[i].line2, LV_TEXT_ALIGN_CENTER, 0);
        // lv_obj_center(label[i].line2);
        // lv_label_set_text(label[i].line2, title[i]);
        lv_label_bind_text(label[i].line2, data[i], "%d");

    }

}
