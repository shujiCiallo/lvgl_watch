#include "screen_card.h"
#include "style/app_colors.h"
#include "style/app_styles.h"
#include "core/data_center.h"

/* 数据标签组:一行标题 + 一行数值 */
typedef struct label_data {
    lv_obj_t *parent;  /* 容器 */
    lv_obj_t *line1;   /* 标题行 */
    lv_obj_t *line2;   /* 数值行 */
} label_data_t;

/* 单个圆环的创建配置 */
typedef struct artivity_rings {
    lv_obj_t *arc;         /* arc 控件 */
    int size;              /* 直径 */
    int width;             /* 弧线宽度 */
    uint32_t max_value;    /* 满量程值 */
    lv_color_t color;      /* 指示条颜色 */
} artivity_rings_t;

/* 数值标签:标签对象 + 满量程,作为 observer user_data 携带 */
typedef struct label_max {
    lv_obj_t *label;   /* 数值标签 */
    int32_t max;       /* 满量程值 */
} label_max_t;

/* 三个活动圆环各自订阅的数据源 */
static lv_subject_t *data[] = {
    &g_calorie_subject,
    &g_steps_subject,
    &g_duration_subject};

/* 三个数值标签的满量程值,顺序与 data[] 对应 */
static const int32_t sport_maxs[] = {400, 6000, 1200};
static label_max_t sport_max[3];

/* 模块内部构建函数 */
static void title_create(screen_card_t *self, lv_obj_t *parent);
static void card_create(screen_card_t *self, lv_obj_t *parent);

static lv_obj_t *artivity_rings_create(screen_card_t *self, lv_obj_t *parent,
    artivity_rings_t *configs);
static lv_obj_t *sport_card_create(screen_card_t *self, lv_obj_t *parent);
static lv_obj_t *label_data_create(lv_obj_t *parent);

static void arc_boserver_cb(lv_observer_t *obs, lv_subject_t *sub);
static void sport_btn_click_cb(lv_event_t *e);
static void label_update_cb(lv_observer_t *obs, lv_subject_t *sub);

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

/* 标题栏:时间 + 日期,均绑定 subject 自动刷新 */
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

/* 卡片区入口 */
static void card_create(screen_card_t *self, lv_obj_t *parent)
{
    sport_card_create(self, parent);
}

/* 运动数据卡片:活动圆环 + 下方数值区 */
static lv_obj_t *sport_card_create(screen_card_t *self, lv_obj_t *parent)
{
    lv_obj_t *sport_card = lv_obj_create(parent);
    lv_obj_set_style_bg_color(sport_card, COLOR_SURFACE, 0);
    lv_obj_set_flex_flow(sport_card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(sport_card, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_coord_t scr_h = lv_disp_get_ver_res(NULL);
    lv_obj_set_size(sport_card, lv_pct(100), (scr_h * 45 / 100));
    lv_obj_add_event_cb(sport_card, sport_btn_click_cb,
        LV_EVENT_PRESSED, self);
    {
        lv_obj_t *meter = lv_btn_create(sport_card);
        lv_obj_clear_flag(meter, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_remove_style_all(meter);
        lv_obj_add_flag(meter, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_flex_grow(meter, 1);
        artivity_rings_t configs[] = {
            {NULL, 60, 10, 400, COLOR_ERROR},
            {NULL, 90, 10, 6000, COLOR_PRIMARY_DARK},
            {NULL, 120, 10, 1200, COLOR_SECONDARY}
        };
        artivity_rings_create(self, meter, configs);

        lv_obj_t *info = lv_obj_create(sport_card);
        lv_obj_remove_style_all(info);
        lv_obj_add_flag(info, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_set_flex_grow(info, 1);
        lv_obj_set_width(info, lv_pct(100));
        lv_obj_set_flex_flow(info, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(info, LV_FLEX_ALIGN_SPACE_EVENLY,
            LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_text_color(info, lv_color_black(), 0);
        lv_obj_set_style_text_font(info, &lv_font_montserrat_18, 0);
        lv_obj_set_style_pad_all(info, 8, 0);

        label_data_create(info);
    }
    return sport_card;
}


/* 活动圆环:三个同心 arc,各自订阅一个数据源 */
static lv_obj_t *artivity_rings_create(screen_card_t *self, lv_obj_t *parent,
     artivity_rings_t *configs)
{
    for (size_t i = 0; i < 3; i++) {
        configs[i].arc = lv_arc_create(parent);

        lv_obj_clear_flag(configs[i].arc, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(configs[i].arc, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        lv_obj_set_event_bubble(configs[i].arc, true);
        lv_obj_set_size(configs[i].arc, configs[i].size, configs[i].size);
        lv_obj_align(configs[i].arc, LV_ALIGN_BOTTOM_MID,
            0, configs[i].size / 2);

        lv_obj_set_style_opa(configs[i].arc, LV_OPA_TRANSP, LV_PART_KNOB);
        lv_obj_set_style_arc_color(configs[i].arc, configs[i].color,
            LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(configs[i].arc, configs[i].width,
            LV_PART_INDICATOR);
        lv_obj_set_style_arc_width(configs[i].arc, configs[i].width,
            LV_PART_MAIN);
        lv_color_t ind_color = configs[i].color;
        lv_color_t main_color = lv_color_darken(ind_color, LV_OPA_40);
        lv_obj_set_style_arc_color(configs[i].arc, main_color,
            LV_PART_MAIN);

        lv_arc_set_bg_angles(configs[i].arc, 180, 0);
        lv_arc_set_max_value(configs[i].arc, configs[i].max_value);
        lv_arc_set_value(configs[i].arc, 0);

        /* 订阅数据变化,订阅句柄存到结构体便于统一移除 */
        self->arc_obs[i] = lv_subject_add_observer(data[i],
            arc_boserver_cb, configs[i].arc);

    }
    return parent;
}

/* 数值信息区:卡路里/步数/次数 三列数据 */
static lv_obj_t *label_data_create(lv_obj_t *parent)
{
    label_data_t label[3] = {};
    size_t num = sizeof(label) / sizeof(label[0]);
    char *title[] = {"calorie", "steps", "times"};
    for (size_t i = 0; i < num; i++) {
        label[i].parent = lv_btn_create(parent);
        lv_obj_remove_style_all(label[i].parent);
        lv_obj_set_flex_grow(label[i].parent, 1);
        lv_obj_set_height(label[i].parent, lv_pct(100));
        lv_obj_set_flex_flow(label[i].parent, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(label[i].parent, LV_FLEX_ALIGN_CENTER,
            LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
        lv_obj_set_event_bubble(label[i].parent, true);

        /* 标题行 */
        label[i].line1 = lv_label_create(label[i].parent);
        lv_obj_set_style_bg_opa(label[i].line1, LV_OPA_COVER, 0);
        lv_obj_set_flex_grow(label[i].line1, 1);
        lv_obj_set_height(label[i].line1, lv_pct(100));
        lv_obj_set_width(label[i].line1, lv_pct(100));
        lv_obj_set_style_text_align(label[i].line1, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_set_text(label[i].line1, title[i]);
        lv_obj_set_event_bubble(label[i].line1, true);

        /* 数值行:绑定 subject,数据更新自动刷新 */
        label[i].line2 = lv_label_create(label[i].parent);
        lv_obj_set_style_bg_opa(label[i].line2, LV_OPA_COVER, 0);
        lv_obj_set_flex_grow(label[i].line2, 1);
        lv_obj_set_height(label[i].line2, lv_pct(100));
        lv_obj_set_width(label[i].line2, lv_pct(100));
        lv_obj_set_style_text_align(label[i].line2, LV_TEXT_ALIGN_CENTER, 0);
        lv_label_bind_text(label[i].line2, data[i], "%d");
        lv_obj_set_event_bubble(label[i].line2, true);
    }
    return parent;
}

/* 圆环数据观察者回调:把 subject 数值写到 arc */
static void arc_boserver_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *arc = lv_observer_get_user_data(obs);
    int16_t value = lv_subject_get_int(sub);
    lv_arc_set_value(arc, value);
}

static void sport_panel_creata(screen_card_t *self, lv_obj_t *parent);
static void sport_info_create(screen_card_t *self, lv_obj_t *parent);
/*sport按键回调*/
static void sport_btn_click_cb(lv_event_t *e)
{
    lv_obj_t *old_scr = lv_scr_act();
    screen_card_t *self = lv_event_get_user_data(e);
    lv_obj_add_flag(old_scr, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t *new = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(new, 8, 0);
    lv_obj_set_size(new, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(new, LV_FLEX_FLOW_COLUMN);

    sport_panel_creata(self, new);

    lv_scr_load(new);
}

static void sport_panel_creata(screen_card_t *self, lv_obj_t *parent)
{
    lv_obj_t *sport_panel = parent;

    lv_obj_t *meter  = lv_obj_create(sport_panel);
    lv_obj_remove_style_all(meter);
    lv_obj_set_size(meter, lv_pct(100), lv_pct(45));
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

lv_style_t sport_label_style;

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

        /* value/max 形式:订阅数据源,更新时按 "当前值/满量程" 刷新 */
        sport_max[i].label = label[i].line2;
        sport_max[i].max = sport_maxs[i];
        lv_subject_add_observer(data[i], label_update_cb, &sport_max[i]);
    }

}

static void label_update_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    label_max_t *lm = lv_observer_get_user_data(obs);
    int32_t cur = lv_subject_get_int(sub);
    lv_label_set_text_fmt(lm->label, "%d/%d", cur, lm->max);
}
