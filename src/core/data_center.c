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

lv_subject_t g_steps_subject;
lv_subject_t g_calorie_subject;
lv_subject_t g_duration_subject;

lv_subject_t g_HR_value_subject;
lv_subject_t g_HR_high_subject;
lv_subject_t g_HR_low_subject;

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

/* 传感器数据读取接口(当前为模拟,预留 MCU 接入) */
static uint16_t get_steps(void);
static uint16_t get_calirie(void);
static uint16_t get_duration(void);
static uint8_t get_battery(void);
static uint16_t get_HR(void);
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
        break;

    case TIMER_ID_M:
        min_const+=1;
        strftime(time_buf, sizeof(time_buf), "%H:%M", info);
        strftime(date_buf, sizeof(date_buf), "%m/%d %a", info);

        lv_subject_copy_string(&g_time_subject, time_buf);
        lv_subject_copy_string(&g_date_subject, date_buf);
        lv_subject_set_int(&g_bat_subject, get_battery());

        /* 每 10 次(10 分钟)采样一次心率,累积本小时最高/最低 */
        if (++hr_sample_cnt >= 10) {
            hr_sample_cnt = 0;
            hr_sample();
        }
        break;

    case TIMER_ID_H:
        hr_rollover();
        break;

    default:
        break;
    }

}

/* 心率采样:累积本小时最高/最低,首次采样直接作为初始区间 */
static void hr_sample(void)
{
    uint16_t hr = get_HR();
    lv_subject_set_int(&g_HR_value_subject, hr);   /* 当前心率,供标题实时显示 */
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

/* 小时切换:发布本小时统计出的最高/最低,并开启新一轮累积 */
static void hr_rollover(void)
{
    if (hr_accum_valid) {
        lv_subject_set_int(&g_HR_low_subject, hr_accum_low);
        lv_subject_set_int(&g_HR_high_subject, hr_accum_high);
        hr_accum_low  = 0;
        hr_accum_high = 0;
        hr_accum_valid = 0;
    }
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
    lv_subject_init_int(&g_duration_subject, 0);
    lv_subject_init_int(&g_HR_value_subject, 0);
    lv_subject_init_int(&g_HR_high_subject, 0);
    lv_subject_init_int(&g_HR_low_subject, 0);

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