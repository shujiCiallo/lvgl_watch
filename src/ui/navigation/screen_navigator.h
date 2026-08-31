#ifndef SCREEN_NAVIGATOR_H
#define SCREEN_NAVIGATOR_H

#include "lvgl.h"

/* 全屏详情页导航:push 压入并显示,pop 销毁并返回上一层。
 * 详情页进入时创建、返回时销毁(数据由 subject 驱动,重建零成本)。
 * 右滑手势统一在此处理返回。 */

void navigator_init(void);
void navigator_push(lv_obj_t *page);
void navigator_pop(void);

#endif
