#include "HRmonitor.h"
#include "style/app_colors.h"
#include "screen/screen_card.h"
#include "lvgl_private.h"   /* 提供 lv_draw_task_t / lv_chart_t 内部结构 */

/* 区间数据:每根柱子的低值(下限)与高值(上限) */
#define POINT_NUM 24
static lv_coord_t low_values[POINT_NUM]  = {60, 65, 73, 59, 58};
static lv_coord_t high_values[POINT_NUM] = {84, 93, 84, 73, 89};
static Uint8 value_index = 0;

static void HR_title_create(lv_obj_t *parent);
static void HR_table_create(lv_obj_t *parent);
static void HR_btn_click_cb(lv_event_t *e);
static void chart_draw_event_cb(lv_event_t *e);

lv_obj_t *HRmonitor_card_create(screen_card_t *self, lv_obj_t *parent)
{
    lv_obj_t *HR_card = lv_obj_create(parent);
    lv_obj_set_style_bg_color(HR_card, COLOR_SURFACE, 0);
    lv_obj_set_flex_flow(HR_card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(HR_card, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_coord_t scr_h = lv_disp_get_ver_res(NULL);
    lv_obj_set_size(HR_card, lv_pct(100), (scr_h * 45 / 100));
    lv_obj_add_event_cb(HR_card, HR_btn_click_cb, LV_EVENT_CLICKED, self);

    HR_title_create(HR_card);

    HR_table_create(HR_card);

    return HR_card;
}

static void HR_title_create(lv_obj_t *parent)
{
    lv_obj_t *HR_title = lv_obj_create(parent);
    lv_obj_remove_style_all(HR_title);
    lv_obj_set_width(HR_title, lv_pct(100));
    lv_obj_set_flex_grow(HR_title, 1);
    lv_obj_set_flex_flow(HR_title, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(HR_title, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START);

    lv_obj_t *HR_label = lv_label_create(HR_title);
    lv_obj_set_style_text_font(HR_label, &lv_font_montserrat_24, 0);
    lv_obj_set_flex_grow(HR_label, 2);
    lv_label_set_text(HR_label, "heart rate");

    lv_obj_t *HR_data_label = lv_label_create(HR_title);
    lv_obj_set_flex_grow(HR_data_label, 3);
    lv_obj_set_style_text_font(HR_data_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_align(HR_data_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_text(HR_data_label, "data  data");
}

static void HR_table_create(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_grow(obj, 3);

    lv_obj_t *chart = lv_chart_create(obj);
    lv_obj_set_size(chart, lv_pct(100), lv_pct(100));
    lv_obj_center(chart);

    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(chart, POINT_NUM);
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 52, 97);

    lv_chart_series_t *ser1 = lv_chart_add_series(chart,
        lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    /* 占位值,实际绘制区域会在回调里被 low/high 区间覆盖 */
    lv_chart_set_all_values(chart, ser1, 50);

    /* 柱子圆角置 0,避免区间很小时圆角吃掉整根柱子 */
    lv_obj_set_style_radius(chart, 0, LV_PART_ITEMS);

    lv_obj_set_send_draw_task_events(chart, true);
    lv_obj_add_event_cb(chart, chart_draw_event_cb,
        LV_EVENT_DRAW_TASK_ADDED, NULL);

    lv_chart_refresh(chart);
}

/* 预留:点击卡片进入心率详情 */
static void HR_btn_click_cb(lv_event_t *e)
{
    (void)e;
}

static void chart_draw_event_cb(lv_event_t *e)
{
    lv_draw_task_t *task = lv_event_get_draw_task(e);
    lv_draw_fill_dsc_t *fill_dsc = lv_draw_task_get_fill_dsc(task);
    if (fill_dsc == NULL) return;

    /* 只处理柱子(LV_PART_ITEMS),跳过背景/分割线等其它绘制任务 */
    if (fill_dsc->base.part != LV_PART_ITEMS) return;

    /* 柱子索引:LVGL 绘制柱子时把它存在 draw_dsc 的 base.id2 */
    int32_t id = fill_dsc->base.id2;
    if (id < 0 || id >= POINT_NUM) return;

    lv_obj_t *chart = lv_event_get_target_obj(e);
    lv_chart_t *c = (lv_chart_t *)chart;
    lv_coord_t y_min = c->ymin[LV_CHART_AXIS_PRIMARY_Y];
    lv_coord_t y_max = c->ymax[LV_CHART_AXIS_PRIMARY_Y];

    /* 图表内容区:与绘制任务同坐标系(相对所在图层) */
    lv_area_t content;
    lv_obj_get_content_coords(chart, &content);
    lv_coord_t h = lv_area_get_height(&content);

    /* Y 轴向下增大,值越大像素 y 越小 */
    lv_coord_t y_high = content.y1 + (h * (y_max - high_values[id]) / (y_max - y_min));
    lv_coord_t y_low  = content.y1 + (h * (y_max - low_values[id]) / (y_max - y_min));

    /* 覆盖默认柱子区域:上边界为高值,下边界为低值,实现区间显示 */
    task->area.y1 = y_high;
    task->area.y2 = y_low;
}

