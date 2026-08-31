#include "data_center.h"
#include <time.h>

#define TIMER_ID_S 1
#define TIMER_ID_M 2
#define TIMER_ID_H 3

/* 全局 subject:UI 通过它们订阅数据,实现数据层与界面解耦 */
lv_subject_t g_time_subject;
lv_subject_t g_date_subject;

lv_subject_t g_bat_subject;
lv_subject_t g_symbol_bat_subject;
lv_subject_t g_compass_subject;

lv_subject_t g_steps_subject;
lv_subject_t g_calorie_subject;
lv_subject_t g_duration_subject;

lv_subject_t g_HR_value_subject;
lv_subject_t g_HR_high_subject;
lv_subject_t g_HR_low_subject;
lv_subject_t g_HR_max24_subject;
lv_subject_t g_HR_min24_subject;

/* 字符串格式化的临时缓冲区 */
static char time_buf[24];
static char date_buf[24];

static lv_timer_t *time_s;
static lv_timer_t *time_m;
static lv_timer_t *time_h;

/* 心率统计:time_m 每 10 分钟采样一次,累积本小时最高/最低;time_h 每小时发布 */
static uint16_t hr_accum_low = 0;
static uint16_t hr_accum_high = 0;
static uint8_t hr_accum_valid = 0;   /* 本小时是否已采样 */
static uint8_t hr_sample_cnt = 0;    /* time_m 计数,每 10 次(10 分钟)采样 */

/* 24小时滚动窗口:缓存每小时的极值,最多 24 个,满了覆盖最旧 */
#define HR24_WINDOW 24
static uint16_t hr24_low_hist[HR24_WINDOW];
static uint16_t hr24_high_hist[HR24_WINDOW];
static uint8_t hr24_cnt = 0;   /* 已缓存的小时数 */
static uint8_t hr24_pos = 0;   /* 下一个写入位置 */

/* 传感器数据读取接口(当前为模拟,预留 MCU 接入) */
static uint16_t get_steps(void);
static uint16_t get_calirie(void);
static uint16_t get_duration(void);
/* 电量图标分级:100 满格 / 64-99 三格 / 34-63 两格 / 0-33 一格 */
const char *bat_symbol_level(uint8_t bat)
{
    if (bat >= 100) return LV_SYMBOL_BATTERY_FULL;
    if (bat >= 64)  return LV_SYMBOL_BATTERY_3;
    if (bat >= 34)  return LV_SYMBOL_BATTERY_2;
    return LV_SYMBOL_BATTERY_1;
}

static uint8_t get_battery(void);
static uint16_t get_HR(void);
static uint16_t get_compass_heading(void);
static void hr_sample(void);
static void hr_rollover(void);

/* 定时回调:每秒更新一次所有 subject */
static void update_time_cb(lv_timer_t *timer) {
    time_t now = time(NULL);
    struct tm *info = localtime(&now);
    int id = (int)(long)lv_timer_get_user_data(timer);
    uint8_t min_const = 0;

    switch (id)
    {
    case TIMER_ID_S:
        lv_subject_set_int(&g_steps_subject, get_steps());
        lv_subject_set_int(&g_calorie_subject, get_calirie());
        lv_subject_set_int(&g_duration_subject, get_duration());
        lv_subject_set_int(&g_compass_subject, get_compass_heading());
            hr_sample();
        break;

    case TIMER_ID_M:
        min_const+=1;
        strftime(time_buf, sizeof(time_buf), "%H:%M", info);
        strftime(date_buf, sizeof(date_buf), "%m/%d %a", info);

        lv_subject_copy_string(&g_time_subject, time_buf);
        lv_subject_copy_string(&g_date_subject, date_buf);
        lv_subject_set_int(&g_bat_subject, get_battery());

        if (++hr_sample_cnt >= 10) {
            hr_sample_cnt = 0;
        }
        break;

    case TIMER_ID_H:
        hr_rollover();
        break;

    default:
        break;
    }

}

/* 累积一次采样到本小时极值,首次采样直接作为初始区间 */
static void hr_accum(uint16_t hr)
{
    if (!hr_accum_valid) {
        hr_accum_low  = hr;
        hr_accum_high = hr;
        hr_accum_valid = 1;
    }
    else {
        if (hr < hr_accum_low)  hr_accum_low  = hr;
        if (hr > hr_accum_high) hr_accum_high = hr;
    }
}

/* 把一小时极值写入24h滚动窗口,满了覆盖最旧 */
static void hr24_push(uint16_t low, uint16_t high)
{
    hr24_low_hist[hr24_pos]  = low;
    hr24_high_hist[hr24_pos] = high;
    hr24_pos = (hr24_pos + 1) % HR24_WINDOW;
    if (hr24_cnt < HR24_WINDOW) hr24_cnt++;
}

/* 重算过去24小时极值并发布:滚动窗口(完整小时) + 当前累积小时 */
static void hr24_publish(uint16_t cur_low, uint16_t cur_high, uint8_t cur_valid)
{
    uint16_t m24_low = 0, m24_high = 0;
    uint8_t have = 0;

    for (uint8_t i = 0; i < hr24_cnt; i++) {
        if (!have || hr24_low_hist[i]  < m24_low)  m24_low  = hr24_low_hist[i];
        if (!have || hr24_high_hist[i] > m24_high) m24_high = hr24_high_hist[i];
        have = 1;
    }
    if (cur_valid) {
        if (!have || cur_low  < m24_low)  m24_low  = cur_low;
        if (!have || cur_high > m24_high) m24_high = cur_high;
        have = 1;
    }
    if (!have) return;   /* 窗口与当前小时均无数据 */

    lv_subject_set_int(&g_HR_min24_subject, m24_low);
    lv_subject_set_int(&g_HR_max24_subject, m24_high);
}

/* 心率采样:累积本小时最高/最低,并同步刷新过去24小时极值 */
static void hr_sample(void)
{
    uint16_t hr = get_HR();
    lv_subject_set_int(&g_HR_value_subject, hr);   /* 当前心率,供标题实时显示 */
    hr_accum(hr);
    hr24_publish(hr_accum_low, hr_accum_high, hr_accum_valid);
}

/* 公开接口:chart 创建时立即采样一次,发布当前区间(画出第一根柱)并初始化24h极值 */
void data_center_hr_sample(void)
{
    uint16_t hr = get_HR();
    lv_subject_set_int(&g_HR_value_subject, hr);
    hr_accum(hr);
    lv_subject_set_int(&g_HR_low_subject, hr_accum_low);
    lv_subject_set_int(&g_HR_high_subject, hr_accum_high);
    hr24_publish(hr_accum_low, hr_accum_high, hr_accum_valid);
}

/* 小时切换:发布本小时统计出的最高/最低,更新24h极值,并开启新一轮累积 */
static void hr_rollover(void)
{
    if (hr_accum_valid) {
        lv_subject_set_int(&g_HR_low_subject, hr_accum_low);
        lv_subject_set_int(&g_HR_high_subject, hr_accum_high);
        hr24_push(hr_accum_low, hr_accum_high);
        hr_accum_low  = 0;
        hr_accum_high = 0;
        hr_accum_valid = 0;
    }
    /* 重置后重算:此时只看完整小时窗口,忽略当前小时 */
    hr24_publish(0, 0, 0);
}

/* 数据层初始化:初始化 subject,并启动每秒刷新的定时器 */
void data_center_init(void) {
    /* 1. 初始化 Subject */
    lv_subject_init_string(&g_time_subject,
                           time_buf,
                           NULL,
                           sizeof(time_buf),
                           "--:--");
    lv_subject_init_string(&g_date_subject,
                           date_buf,
                           NULL,
                           sizeof(date_buf),
                           "-/-");
    lv_subject_init_int(&g_steps_subject, 0);
    lv_subject_init_int(&g_calorie_subject, 0);
    lv_subject_init_int(&g_bat_subject, get_battery());
    lv_subject_init_int(&g_compass_subject, get_compass_heading());
    lv_subject_init_int(&g_duration_subject, 0);
    lv_subject_init_int(&g_HR_value_subject, 0);
    lv_subject_init_int(&g_HR_high_subject, 0);
    lv_subject_init_int(&g_HR_low_subject, 0);
    lv_subject_init_int(&g_HR_max24_subject, 0);
    lv_subject_init_int(&g_HR_min24_subject, 0);

    /* 2. 启动定时器,每秒触发一次 update_time_cb */
    time_s = lv_timer_create(update_time_cb,
        1000, (void *)(long)TIMER_ID_S);
    time_m = lv_timer_create(update_time_cb,
        1000 * 60, (void *)(long)TIMER_ID_M);
    update_time_cb(time_m);
    time_h = lv_timer_create(update_time_cb,
        1000 * 60 * 60, (void *)(long)TIMER_ID_H);
    update_time_cb(time_h);


}

/* 步数模拟读数 */
static uint16_t get_steps(void)
{
    ///MCU 传感器预留接口
    static uint16_t steps = 0;
    steps += 10;
    return steps;
}

/* 卡路里模拟读数 */
static uint16_t get_calirie(void)
{
    ///MCU 传感器预留接口
    static uint16_t cal = 0;
    cal += 5;
    return cal;
}

/* 运动时长模拟读数 */
static uint16_t get_duration(void)
{
    ///MCU 传感器预留接口
    static uint16_t dur = 0;
    dur += 1;
    return dur;
}

static uint8_t get_battery(void)
{
    static uint8_t bat = 102;
    bat -= 1;
    return bat;
}

static uint16_t get_HR(void)
{
    ///MCU 传感器预留接口
    static uint16_t hr = 60;
    hr = (uint16_t)(60 + (hr * 7 + 13) % 37);  /* 线性同余伪随机*/
    return hr;
}

/* 罗盘磁航向模拟读数:0-359° 顺时针缓慢旋转 */
static uint16_t get_compass_heading(void)
{
    ///MCU 传感器预留接口
    static uint16_t heading = 0;
    heading = (uint16_t)((heading + 3) % 360);
    return heading;
}