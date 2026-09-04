#include "app_fonts.h"

#define APP_FONT_CJK_PATH   "fonts/simhei.ttf"
#define APP_FONT_CJK_SLOTS  4

typedef struct {
    int32_t size;
    lv_font_t *font;
}cjk_slot_t;

/* 共享字体资源缓存(非组件状态,允许文件级 static) */
static cjk_slot_t s_cjk[APP_FONT_CJK_SLOTS];

lv_font_t * app_font_cjk(int32_t size)
{
    int free_slot = -1;
    for (int i = 0; i < APP_FONT_CJK_SLOTS; i++) {
        if (s_cjk[i].font != NULL && s_cjk[i].size == size) {
            return s_cjk[i].font;
        }
        if (s_cjk[i].font == NULL && free_slot < 0) free_slot = i;
    }
    if (free_slot < 0) free_slot = 0;   /* 槽满:覆盖首个(很少见) */

    lv_font_t *font = lv_freetype_font_create(APP_FONT_CJK_PATH,
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP, (uint32_t)size,
        LV_FREETYPE_FONT_STYLE_NORMAL);
    if (font == NULL) return NULL;

    s_cjk[free_slot].size = size;
    s_cjk[free_slot].font = font;
    return font;
}
