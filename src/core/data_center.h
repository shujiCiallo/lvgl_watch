#ifndef DATA_CENTER_H
#define DATA_CENTER_H

#include "lvgl.h"

/* 卡路里满量程:UI 的 slider/arc 最大值与数据层保持一致 */
#define CALORIE_MAX 400

extern lv_subject_t g_time_subject;
extern lv_subject_t g_date_subject;

extern lv_subject_t g_bat_subject;
extern lv_subject_t g_symbol_bat_subject;
extern lv_subject_t g_compass_subject;   /* 磁航向(度) 0-359 */

extern lv_subject_t g_steps_subject;
extern lv_subject_t g_calorie_subject;
extern lv_subject_t g_duration_subject;

extern lv_subject_t g_HR_value_subject;   /* 最近一次检测到的心率(bpm) */
extern lv_subject_t g_HR_high_subject;
extern lv_subject_t g_HR_low_subject;
extern lv_subject_t g_HR_max24_subject;   /* 过去24小时内心率最大值 */
extern lv_subject_t g_HR_min24_subject;   /* 过去24小时内心率最小值 */

void data_center_init(void);
void data_center_hr_sample(void);   /* 立即采样一次心率,chart 创建时调用 */

#endif
