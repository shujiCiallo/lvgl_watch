#ifndef __EVENT_H
#define __EVENT_H

#include <time.h>

/* 事件处理模块。
 * 目前暂无启用的事件逻辑;时间/日期的周期刷新已迁移到
 * core/data_center 的 subject 机制,事件回调后续在此扩展。 */

/* 以下为历史实现参考,已由 subject 方案替代:
// extern time_t current_time;
// extern struct tm *local_time;
// extern char time_buf[24];

// void time_cb(lv_timer_t *time);
*/

#endif
