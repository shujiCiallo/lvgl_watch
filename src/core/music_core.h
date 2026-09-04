#ifndef MUSIC_CORE_H
#define MUSIC_CORE_H

#include <stdint.h>
#include "lvgl.h"

/* 音乐播放内核(单例数据层):歌单扫描 + 播放状态机 + subject 发布。
 * UI 只发命令/订阅 subject,不直接读写播放状态。
 * 第 1 步:lv_timer 秒级模拟推进;接真解码时替换内部数据源,subject/API 不变。 */

#define MUSIC_MAX_SONGS       100
#define MUSIC_SONG_NAME_LEN   128
#define MUSIC_SONG_PATH_LEN   512

typedef struct {
    char path[MUSIC_SONG_PATH_LEN];
    char name[MUSIC_SONG_NAME_LEN];
    uint16_t duration_sec;   /* 0 = 时长未知 */
}music_song_t;

typedef enum {
    MUSIC_STATE_IDLE,
    MUSIC_STATE_PLAYING,
    MUSIC_STATE_PAUSED,
    MUSIC_STATE_STOPPED,
}music_state_t;

typedef enum {
    MUSIC_MODE_SEQUENTIAL,   /* 顺序播,播完列表停 */
    MUSIC_MODE_LOOP_ALL,     /* 列表循环 */
    MUSIC_MODE_LOOP_ONE,     /* 单曲循环 */
    MUSIC_MODE_SHUFFLE,      /* 随机 */
}music_mode_t;

/* 发布:UI 经 subject 消费(曲名/进度 arc/状态图标/模式) */
extern lv_subject_t g_music_state_subject;      /* int: music_state_t */
extern lv_subject_t g_music_mode_subject;       /* int: music_mode_t */
extern lv_subject_t g_music_title_subject;      /* string: 当前曲名 */
extern lv_subject_t g_music_pos_subject;        /* int: 已播秒数 */
extern lv_subject_t g_music_duration_subject;   /* int: 当前曲总秒数 */
extern lv_subject_t g_music_index_subject;      /* int: 当前曲在歌单中的下标(0-based) */
extern lv_subject_t g_music_volume_subject;     /* int: 音量 0~100 */

/* 命令:UI 只调用这些 */
void music_core_init(void);
uint16_t music_core_song_count(void);
const music_song_t *music_core_song(uint16_t index);
void music_core_play_index(int index);   /* 切到指定曲并播放 */
void music_core_toggle(void);            /* 播放/暂停 */
void music_core_next(void);
void music_core_prev(void);
void music_core_cycle_mode(void);        /* 顺序轮换播放模式 */
void music_core_set_volume(int v);       /* 音量 0~100 */

#endif
