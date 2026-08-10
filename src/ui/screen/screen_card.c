#include "screen_card.h"
#include "screen/card/card.h"
#include "style/app_styles.h"
#include "core/data_center.h"

/* 标题栏:时间 + 日期,均绑定 subject 自动刷新 */
static void title_create(screen_card_t *self, lv_obj_t *parent);

/* 创建卡片屏幕:标题栏 + 运动数据卡片 */
void screen_card_create(screen_card_t *self, lv_obj_t *parent)
{
    self->root = parent;   /* 保存根对象到结构体 */
    lv_obj_set_style_pad_all(self->root, 8, 0);
    lv_obj_set_flex_flow(self->root, LV_FLEX_FLOW_COLUMN);

    lv_obj_t *title = lv_obj_create(self->root);
    lv_obj_remove_style_all(title);
    lv_obj_set_size(title, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(title, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title, LV_FLEX_ALIGN_SPACE_BETWEEN,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_flex_grow(title, 0);

    title_create(self, title);

    lv_obj_t *card_panel = lv_obj_create(self->root);
    lv_obj_remove_style_all(card_panel);
    lv_obj_set_flex_flow(card_panel, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(card_panel, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_flex_grow(card_panel, 1);
    card_create(self, card_panel);
    lv_obj_set_size(card_panel, lv_pct(100), LV_SIZE_CONTENT);
}

static void title_create(screen_card_t *self, lv_obj_t *parent)
{
    /* 标题样式存入结构体,替代模块级全局静态变量 */
    lv_style_init(&self->title_style);
    lv_style_set_text_font(&self->title_style, &lv_font_montserrat_24);

    lv_obj_add_style(parent, &self->title_style, 0);
    {
        lv_obj_t *time_label = lv_label_create(parent);
        lv_label_bind_text(time_label, &g_time_subject, "%s");

        lv_obj_t *date = lv_label_create(parent);
        lv_label_bind_text(date, &g_date_subject, "%s");
    }
}
