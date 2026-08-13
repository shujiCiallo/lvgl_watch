#include "HRmonitor.h"
#include "style/app_colors.h"
#include "screen/screen_card.h"
#include "core/data_center.h"
#include "lvgl_private.h"   /* 提供 lv_draw_task_t / lv_chart_t 内部结构 */

/* 区间数据:每根柱子的低值(下限)与高值(上限) */
#define POINT_NUM 24
#define HR_AXIS_PADDING 5   /* Y 轴范围在24h极值上下的边距,避免柱子贴边 */
static lv_coord_t low_values[POINT_NUM]  = {};
static lv_coord_t high_values[POINT_NUM] = {};
static Uint8 value_index = 0;

static void HR_title_create(lv_obj_t *parent);
static void HR_table_create(lv_obj_t *parent);
static void HR_btn_click_cb(lv_event_t *e);
static void chart_draw_event_cb(lv_event_t *e);
static void HR_observer_cb(lv_observer_t *obs, lv_subject_t *sub);
static void HR_range_observer_cb(lv_observer_t *obs, lv_subject_t *sub);

lv_obj_t *HRmonitor_card_create(screen_card_t *self, lv_obj_t *parent)
{
    lv_obj_t *HR_card = lv_obj_create(parent);
    lv_obj_set_style_bg_color(HR_card, COLOR_SURFACE, 0);
    lv_obj_set_style_pad_all(HR_card, 5, 0);
    lv_obj_set_flex_flow(HR_card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(HR_card, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_coord_t scr_h = lv_disp_get_ver_res(NULL);
    lv_obj_set_size(HR_card, lv_pct(100), (scr_h * 45 / 100));
    lv_obj_add_event_cb(HR_card, HR_btn_click_cb, LV_EVENT_CLICKED, self);

    HR_title_create(HR_card);

    HR_table_create(HR_card);

    lv_obj_t *HR_time = lv_obj_create(HR_card);
    lv_obj_remove_style_all(HR_time);
    lv_obj_set_width(HR_time, lv_pct(100));
    lv_obj_set_flex_grow(HR_time, 1);
    lv_obj_set_flex_flow(HR_time, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(HR_time, LV_FLEX_ALIGN_SPACE_BETWEEN, 
        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(HR_time, LV_OBJ_FLAG_SCROLLABLE);
    
    lv_obj_t *_00 = lv_label_create(HR_time);
    lv_obj_set_width(_00, lv_pct(25));
    lv_obj_set_style_bg_color(_00, COLOR_ERROR, 0);
    lv_label_set_text(_00, "00:00");
    lv_obj_t *_24 = lv_label_create(HR_time);
    lv_obj_set_width(_24, lv_pct(25));
    lv_label_set_text(_24, "24:00");

    return HR_card;
}

static void HR_title_create(lv_obj_t *parent)
{
    lv_obj_t *HR_title = lv_obj_create(parent);
    lv_obj_remove_style_all(HR_title);
    lv_obj_set_width(HR_title, lv_pct(100));
    lv_obj_set_flex_grow(HR_title, 2);
    lv_obj_set_flex_flow(HR_title, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(HR_title, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START);
    lv_obj_remove_flag(HR_title, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *HR_label = lv_label_create(HR_title);
    lv_obj_set_style_text_font(HR_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(HR_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_set_flex_grow(HR_label, 3);
    lv_label_set_text(HR_label, "heart rate");

    /* 右侧数据区:当前心率(bpm) + 最近更新时间 */
    lv_obj_t *HR_data_box = lv_obj_create(HR_title);
    lv_obj_remove_style_all(HR_data_box);
    lv_obj_set_flex_grow(HR_data_box, 2);
    lv_obj_set_flex_flow(HR_data_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(HR_data_box, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);

    lv_obj_t *HR_bpm_label = lv_label_create(HR_data_box);
    lv_obj_set_style_text_font(HR_bpm_label, &lv_font_montserrat_18, 0);
    lv_label_bind_text(HR_bpm_label, &g_HR_value_subject, "%d bpm");

    lv_obj_t *HR_time_label = lv_label_create(HR_data_box);
    lv_obj_set_style_text_font(HR_time_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_opa(HR_time_label, LV_OPA_50, 0);
    lv_label_bind_text(HR_time_label, &g_time_subject, "at %s");
}

static void HR_table_create(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    // lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_flex_grow(obj, 4);
    lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_CENTER,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);

    lv_obj_t *chart = lv_chart_create(obj);
    // lv_obj_set_size(chart, lv_pct(100), lv_pct(90));
    lv_obj_set_height(chart, lv_pct(100));
    // lv_obj_center(chart);
    lv_obj_set_flex_grow(chart, 7);


    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(chart, POINT_NUM);
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, 52, 97);

    lv_chart_series_t *ser1 = lv_chart_add_series(chart,
        lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    /* 占位值,实际绘制区域会在回调里被 low/high 区间覆盖 */
    lv_chart_set_all_values(chart, ser1, 50);

    /* 柱子圆角置 0,避免区间很小时圆角吃掉整根柱子 */
    lv_obj_set_style_radius(chart, 0, LV_PART_ITEMS);
    lv_obj_set_style_border_width(chart, 0, 0);

    lv_obj_set_send_draw_task_events(chart, true);
    lv_obj_add_event_cb(chart, chart_draw_event_cb,
        LV_EVENT_DRAW_TASK_ADDED, NULL);
    /* 订阅心率区间:data_center 每次采样先发 low 再发 high,回调据此写入同一柱 */
    lv_subject_add_observer(&g_HR_low_subject, HR_observer_cb, chart);
    lv_subject_add_observer(&g_HR_high_subject, HR_observer_cb, chart);
    /* 订阅24h极值:同步更新 Y 轴范围 */
    lv_subject_add_observer(&g_HR_min24_subject, HR_range_observer_cb, chart);
    lv_subject_add_observer(&g_HR_max24_subject, HR_range_observer_cb, chart);

    lv_chart_refresh(chart);

    lv_obj_t *range_data = lv_obj_create(obj);
    lv_obj_remove_style_all(range_data);
    lv_obj_set_flex_grow(range_data, 1);
    lv_obj_set_style_pad_all(range_data, 2, 0);
    lv_obj_set_height(range_data, lv_pct(100));
    lv_obj_set_flex_flow(range_data, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(range_data, LV_FLEX_ALIGN_SPACE_BETWEEN, 
        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t *range_high = lv_label_create(range_data);
    lv_label_bind_text(range_high, &g_HR_max24_subject, "%d");
    lv_obj_t *range_low = lv_label_create(range_data);
    lv_label_bind_text(range_low, &g_HR_min24_subject, "%d");

    /* chart 创建时立即获取一次心率,画出第一根柱并初始化24h极值 */
    data_center_hr_sample();
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

static void HR_observer_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    lv_obj_t *chart = lv_observer_get_user_data(obs);
    int16_t new_value = lv_subject_get_int(sub);

    if (sub == &g_HR_high_subject) {
        high_values[value_index] = new_value;
        /* 一次采样的 high/low 写入同一根柱子,high 最后到达负责推进下标 */
        value_index = (value_index + 1) % POINT_NUM;
    }
    else {
        low_values[value_index] = new_value;
    }

    lv_chart_refresh(chart);
}

/* 24h极值观察者回调:同步 chart Y 轴范围为 [min24-pad, max24+pad] */
static void HR_range_observer_cb(lv_observer_t *obs, lv_subject_t *sub)
{
    (void)sub;
    lv_obj_t *chart = lv_observer_get_user_data(obs);
    int32_t max24 = lv_subject_get_int(&g_HR_max24_subject);
    int32_t min24 = lv_subject_get_int(&g_HR_min24_subject);
    if (max24 == 0) return;   /* 尚无24h极值,保持默认轴范围 */

    lv_coord_t y_min = min24 - HR_AXIS_PADDING;
    if (y_min < 0) y_min = 0;
    lv_chart_set_axis_range(chart, LV_CHART_AXIS_PRIMARY_Y, y_min, max24 + HR_AXIS_PADDING);
    lv_chart_refresh(chart);
}