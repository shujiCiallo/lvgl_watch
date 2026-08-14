#ifndef __EVENT_H
#define __EVENT_H

#include "lvgl.h"
#include "screen/screen_card.h"

/* 通用事件处理模块。
 * sport 详情页的打开/关闭逻辑集中于此,供各界面点击复用。 */

/* 点击打开运动详情页:注册到任意可点击控件,user_data 传 screen_card_t* */
void sport_btn_click_cb(lv_event_t *e);

#endif
