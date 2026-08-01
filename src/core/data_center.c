#include "data_center.h"
#include <time.h>

// 定义 Subject 和存储字符串的缓冲区
lv_subject_t g_time_subject;
static char time_buf[24];

// 定时器回调：更新时间数据
static void update_time_cb(lv_timer_t *timer) {
    time_t now = time(NULL);
    struct tm *info = localtime(&now);

    strftime(time_buf, sizeof(time_buf), "%H:%M", info);

    // 更新 Subject（这将自动触发所有绑定的Label刷新）
    lv_subject_copy_string(&g_time_subject, time_buf);
}

// 模块初始化：启动定时器，初始化Subject
void data_center_init(void) {
    // 1. 初始化 Subject
    lv_subject_init_string(&g_time_subject,
                           time_buf,
                           NULL,
                           sizeof(time_buf),
                           "--:--"); // 初始值

    // 2. 创建一个定时器，每秒触发一次 update_time_cb
    lv_timer_create(update_time_cb, 1000, NULL);
}
