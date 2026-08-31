#include "navigation/screen_navigator.h"
#include "ui_utils.h"

#define NAV_STACK_MAX 8

typedef struct {
    lv_obj_t *stack[NAV_STACK_MAX];   /* 被覆盖(压栈)的屏,栈顶=最近进入详情页前的屏 */
    uint16_t depth;                   /* 已压栈的屏数量 */
} nav_t;

static nav_t s_nav;

/* 统一手势返回:右滑 pop,销毁当前详情页并恢复上一层 */
static void nav_gesture_cb(lv_event_t *e)
{
    (void)e;
    lv_dir_t dir = lv_indev_get_gesture_dir(lv_indev_active());
    if (dir != LV_DIR_RIGHT) return;
    navigator_pop();
}

void navigator_init(void)
{
    s_nav.depth = 0;
}

/* 压入一个全屏详情页并显示;当前屏入栈保存,返回时销毁详情页并恢复 */
void navigator_push(lv_obj_t *page)
{
    if (s_nav.depth >= NAV_STACK_MAX) return;

    lv_obj_t *cur = lv_scr_act();
    s_nav.stack[s_nav.depth++] = cur;

    lv_obj_set_hidden(cur, true);
    lv_scr_load(page);

    lv_obj_add_event_cb(page, nav_gesture_cb, LV_EVENT_GESTURE, NULL);
    enable_bubble_all(page);
}

/* 返回:销毁当前详情页,恢复上一层屏 */
void navigator_pop(void)
{
    if (s_nav.depth == 0) return;

    lv_obj_t *page = lv_scr_act();
    lv_obj_t *prev = s_nav.stack[--s_nav.depth];

    lv_obj_delete(page);
    lv_obj_set_hidden(prev, false);
    lv_scr_load(prev);
}
