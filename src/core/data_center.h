#ifndef DATA_CENTER_H
#define DATA_CENTER_H

#include "lvgl.h"

extern lv_subject_t g_time_subject;
extern lv_subject_t g_date_subject;

extern lv_subject_t g_bat_subject;
extern lv_subject_t g_symbol_bat_subject;

extern lv_subject_t g_steps_subject;
extern lv_subject_t g_calorie_subject;
extern lv_subject_t g_duration_subject;

extern lv_subject_t g_HR_value_subject;   /* 最近一次检测到的心率(bpm) */
extern lv_subject_t g_HR_high_subject;
extern lv_subject_t g_HR_low_subject;

void data_center_init(void);

#endif
