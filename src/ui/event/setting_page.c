#include "setting_page.h"
#include "style/app_colors.h"
#include "lvgl.h"
#include "screen/screen_manager.h"
#include "core/data_center.h"
#include "navigation/screen_navigator.h"

typedef struct {
    lv_obj_t *root;
}setting_page_t;

static setting_page_t s_setting_page;

static void setting_page_create(setting_page_t *self);
static void title_create(lv_obj_t *parent);
static void options_create(lv_obj_t *parent);

void setting_btn_click_cb(lv_event_t *e)
{
    (void)e;
    setting_page_create(&s_setting_page);
    navigator_push(s_setting_page.root);
}

static void setting_page_create(setting_page_t *self)
{
    self->root = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(self->root, 8, 0);
    lv_obj_set_size(self->root, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(self->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_scrollable(self->root, false);

    lv_obj_t *title = lv_obj_create(self->root);
    lv_obj_remove_style_all(title);
    lv_obj_set_flex_grow(title, 1);
    lv_obj_set_width(title, lv_pct(100));
    lv_obj_set_flex_flow(title, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title, LV_FLEX_ALIGN_SPACE_BETWEEN,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_all(title, 0, 0);

    title_create(title);

    lv_obj_t *option = lv_obj_create(self->root);
    lv_obj_remove_style_all(option);
    lv_obj_set_flex_grow(option, 12);
    lv_obj_set_width(option, lv_pct(100));
    lv_obj_set_flex_flow(option, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(option, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    options_create(option);

}

static void title_create(lv_obj_t *parent)
{
    static lv_style_t title_style;

    lv_style_init(&title_style);
    lv_style_set_text_font(&title_style, &lv_font_montserrat_28);

    lv_obj_t *name = lv_label_create(parent);
    lv_label_set_text(name, "setting");
    lv_obj_add_style(name, &title_style, 0);

    lv_obj_t *time = lv_label_create(parent);
    lv_obj_add_style(time, &title_style, 0);
    lv_label_bind_text(time, &g_time_subject, "%s");

}

static void options_create(lv_obj_t *parent)
{
    static lv_style_t options_style = {};

    lv_style_init(&options_style);
    lv_style_set_text_font(&options_style, &lv_font_montserrat_30);
    lv_style_set_text_align(&options_style, LV_TEXT_ALIGN_LEFT);

    Button_t options[] = {
       {.text = "Device"},
       {.text = "Notifications"},
       {.text = "WatchFaces"},
       {.text = "Haptics"},
       {.text = "AppManagement"},
       {.text = "PowerSaving"},
       {.text = "ActivitySetting"},
       {.text = "Preferences"},
    };

    size_t num = sizeof(options) / sizeof(options[0]);
    for (int i = 0; i < num; i++)
    {
        options[i].btn = lv_obj_create(parent);
        lv_obj_set_width(options[i].btn, lv_pct(100));
        lv_obj_set_height(options[i].btn, lv_pct(25));
        lv_obj_set_style_bg_color(options[i].btn, COLOR_SURFACE, 0);
        lv_obj_set_style_margin_all(options[i].btn, 4, 0);

        options[i].label = lv_label_create(options[i].btn);
        lv_obj_add_style(options[i].label, &options_style, 0);
        lv_obj_set_size(options[i].label, lv_pct(100), LV_SIZE_CONTENT);
        lv_obj_center(options[i].label);
        lv_label_set_text(options[i].label, options[i].text);
        lv_obj_set_style_bg_color(options[i].label, lv_palette_main(LV_PALETTE_BLUE), 0);
    }
}
