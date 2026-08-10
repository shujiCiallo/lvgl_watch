#ifndef CARD_H
#define CARD_H

#include "screen/screen_card.h"

/* 创建运动卡片区域:卡片 + 运动详情页,状态写入 self,控件挂在 parent 下 */
void card_create(screen_card_t *self, lv_obj_t *parent);

#endif
