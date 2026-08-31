#include "sport_panel.h"
#include "style/app_colors.h"
#include "lvgl.h"
#include "screen/screen_card.h"
#include "screen/card/sport_card.h"

panel_t sport_panel = {};

void sport_panel_creata(lv_obj_t *parent)
{
    lv_obj_t *sport_panel = parent;

    lv_obj_t *meter  = lv_obj_create(sport_panel);
    lv_obj_remove_style_all(meter);
    lv_obj_set_size(meter, lv_pct(100), lv_pct(45));
    lv_obj_set_scrollable(meter, false);
    artivity_rings_t configs[] = {
        {NULL, 140, 27, 400, COLOR_ERROR},
        {NULL, 210, 27, 6000, COLOR_PRIMARY_DARK},
        {NULL, 280, 27, 1200, COLOR_SECONDARY}
    };
    artivity_rings_create(self, meter, configs);

    lv_obj_t *info = lv_obj_create(sport_panel);
    lv_obj_remove_style_all(info);
    lv_obj_set_size(info, lv_pct(100), lv_pct(50));
    lv_obj_set_flex_flow(info, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(info, 8, 0);

    sport_info_create(self, info);
}

static void sport_info_create(screen_card_t *self, lv_obj_t *parent)
{
    lv_obj_t *info = parent;
    label_data_t label[3] = {};
    size_t num = sizeof(label) / sizeof(label[0]);
    char *title[] = {"calorie", "steps", "times"};

    lv_style_init(&sport_label_style);
    lv_style_set_text_font(&sport_label_style,
        &lv_font_montserrat_28);

    for (size_t i = 0; i < num; i++) {
        label[i].parent = lv_obj_create(info);
        lv_obj_remove_style_all(label[i].parent);
        lv_obj_set_size(label[i].parent, lv_pct(100), lv_pct(30));
        lv_obj_set_flex_flow(label[i].parent, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(label[i].parent, LV_FLEX_ALIGN_START,
            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_add_style(label[i].parent, &sport_label_style, 0);

        label[i].line1 = lv_label_create(label[i].parent);
        lv_label_set_text(label[i].line1, title[i]);
        lv_obj_set_flex_grow(label[i].line1, 3);
        lv_obj_set_style_text_align(label[i].line1, LV_TEXT_ALIGN_CENTER, 0);

        label[i].line2 = lv_label_create(label[i].parent);
        lv_obj_set_flex_grow(label[i].line2, 4);
        lv_obj_set_style_text_align(label[i].line2, LV_TEXT_ALIGN_CENTER, 0);

        /* value/max 形式:绑定对象订阅,label 删除时 observer 自动移除 */
        sport_max[i].label = label[i].line2;
        sport_max[i].max = sport_maxs[i];
        lv_subject_add_observer_obj(data[i], label_update_cb,
            label[i].line2, &sport_max[i]);
    }

}
