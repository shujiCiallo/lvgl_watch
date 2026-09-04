#ifndef APP_FONTS_H
#define APP_FONTS_H

#include "lvgl/lvgl.h"

/* 动态 CJK 字体(freetype + 项目内 simhei.ttf),供中文/日文曲名、歌单等。
 * 同尺寸首次创建后缓存复用;字库缺失/加载失败返回 NULL,调用方回退内置字体。 */
lv_font_t * app_font_cjk(int32_t size);

#endif
