#ifndef MUSIC_H
#define MUSIC_H

#include "lvgl.h"
#include "core/music_core.h"

/* 音乐详情页:进入创建返回销毁,数据全部由 core subject 驱动。
 * 页内含三个全屏子视图(list 歌单 / player 播放页 / volume 音量),
 * 经 lv_obj_set_hidden 互斥切换;二级页右滑返回播放页。 */
typedef struct {
    lv_obj_t *root;
    lv_obj_t *view_list;    /* 子视图:歌单列表 */
    lv_obj_t *view_player;  /* 子视图:播放页(默认) */
    lv_obj_t *view_volume;  /* 子视图:音量调节 */
    lv_obj_t *label;        /* 播放页:顶部曲名区 */
    lv_obj_t *btn;          /* 播放页:播放控制区(进度环 + 上/播/下) */
    lv_obj_t *tools;        /* 播放页:工具行(列表/音量/模式) */
    lv_obj_t *list_rows;    /* 歌单列表:可滚动行容器 */
}music_panel_t;

void music_panel_create(music_panel_t *self);
/* navigator_push(self->root) 之后调用:把 list/volume 二级页的右滑收回页内
 * (返回播放页),不触发导航栈 pop;player 页右滑仍由 navigator 返回上级 */
void music_panel_gesture_install(music_panel_t *self);

#endif
