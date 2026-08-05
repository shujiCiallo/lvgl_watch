#include "data_center.h"
#include <time.h>

/* 全局 subject:UI 通过它们订阅数据,实现数据层与界面解耦 */
lv_subject_t g_time_subject;
lv_subject_t g_date_subject;

lv_subject_t g_steps_subject;
lv_subject_t g_calorie_subject;
lv_subject_t g_duration_subject;

/* 字符串格式化的临时缓冲区 */
static char time_buf[24];
static char date_buf[24];

/* 传感器数据读取接口(当前为模拟,预留 MCU 接入) */
static uint16_t get_steps(void);
static uint16_t get_calirie(void);
static uint16_t get_duration(void);

/* 定时回调:每秒更新一次所有 subject */
static void update_time_cb(lv_timer_t *timer) {
    time_t now = time(NULL);
    struct tm *info = localtime(&now);

    strftime(time_buf, sizeof(time_buf), "%H:%M", info);
    strftime(date_buf, sizeof(date_buf), "%m/%d %a", info);

    lv_subject_copy_string(&g_time_subject, time_buf);
    lv_subject_copy_string(&g_date_subject, date_buf);
    lv_subject_set_int(&g_steps_subject, get_steps());
    lv_subject_set_int(&g_calorie_subject, get_calirie());
    lv_subject_set_int(&g_duration_subject, get_duration());
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
    lv_subject_init_int(&g_duration_subject, 0);

    /* 2. 启动定时器,每秒触发一次 update_time_cb */
    lv_timer_create(update_time_cb, 1000, NULL);
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
