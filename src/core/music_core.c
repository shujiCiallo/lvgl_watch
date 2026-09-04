#include "music_core.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

/* 内核为单例数据层服务,持有文件级状态(同 data_center),UI 经 subject 消费 */
static music_song_t s_songs[MUSIC_MAX_SONGS];
static uint16_t s_song_count;
static uint16_t s_index;
static music_state_t s_state;
static music_mode_t s_mode;
static uint16_t s_pos;
static uint8_t s_volume;
static char s_title[MUSIC_SONG_NAME_LEN];   /* 兼作 string subject 内部缓冲 */
static uint32_t s_rnd = 0x12345678;          /* 伪随机(播放模式 SHUFFLE 用) */

lv_subject_t g_music_state_subject;
lv_subject_t g_music_mode_subject;
lv_subject_t g_music_title_subject;
lv_subject_t g_music_pos_subject;
lv_subject_t g_music_duration_subject;
lv_subject_t g_music_index_subject;
lv_subject_t g_music_volume_subject;

static int scan_music_folder(const char *dir_path, music_song_t *songs, int max_songs);
static uint16_t fake_duration(const char *name);

/* ---- 内部发布 ---- */

static void publish_pos(void)
{
    lv_subject_set_int(&g_music_pos_subject, s_pos);
}

static void set_state(music_state_t st)
{
    s_state = st;
    lv_subject_set_int(&g_music_state_subject, (int)st);
}

/* 跳到指定曲并复位进度:发布标题/时长,不改播放状态 */
static void goto_index(int i)
{
    if (s_song_count == 0) return;
    if (i < 0) i = 0;
    else if (i >= (int)s_song_count) i = s_song_count - 1;

    s_index = (uint16_t)i;
    s_pos = 0;
    snprintf(s_title, sizeof(s_title), "%s", s_songs[s_index].name);
    lv_subject_copy_string(&g_music_title_subject, s_title);
    lv_subject_set_int(&g_music_duration_subject, s_songs[s_index].duration_sec);
    lv_subject_set_int(&g_music_index_subject, (int)s_index);
    publish_pos();
}

static void play_index_at(int i)
{
    goto_index(i);
    set_state(MUSIC_STATE_PLAYING);
}

static int random_index(void)
{
    s_rnd = s_rnd * 1664525u + 1013904223u;
    return (int)(s_rnd % s_song_count);
}

/* 一首播完:按播放模式决定下一首;LOOP_ONE 原地重播,SEQUENTIAL 末尾停 */
static void on_song_finished(void)
{
    switch (s_mode) {
    case MUSIC_MODE_SEQUENTIAL:
        if (s_index == s_song_count - 1) {
            set_state(MUSIC_STATE_STOPPED);
        } else {
            play_index_at(s_index + 1);
        }
        break;
    case MUSIC_MODE_LOOP_ALL:
        play_index_at((s_index + 1) % s_song_count);
        break;
    case MUSIC_MODE_LOOP_ONE:
        s_pos = 0;
        publish_pos();
        set_state(MUSIC_STATE_PLAYING);
        break;
    case MUSIC_MODE_SHUFFLE:
        play_index_at(random_index());
        break;
    }
}

/* 模拟播放源:秒级推进;接真解码时由解码回调按真实时间驱动同一 subject */
static void music_tick_cb(lv_timer_t *timer)
{
    (void)timer;
    if (s_song_count == 0 || s_state != MUSIC_STATE_PLAYING) return;

    uint16_t dur = s_songs[s_index].duration_sec;
    if (dur == 0) return;
    if (s_pos < dur) {
        s_pos++;
        publish_pos();
    }
    if (s_pos >= dur) on_song_finished();
}

/* ---- 命令接口 ---- */

uint16_t music_core_song_count(void)
{
    return s_song_count;
}

const music_song_t *music_core_song(uint16_t index)
{
    if (index >= s_song_count) return NULL;
    return &s_songs[index];
}

void music_core_play_index(int index)
{
    if (s_song_count == 0) return;
    if (index < 0) index = 0;
    else if (index >= (int)s_song_count) index = s_song_count - 1;
    play_index_at(index);
}

void music_core_toggle(void)
{
    if (s_song_count == 0) return;
    if (s_state == MUSIC_STATE_PLAYING) {
        set_state(MUSIC_STATE_PAUSED);
        return;
    }
    /* 从暂停/停止/结束处恢复;若已播到末尾则重头播 */
    if (s_pos >= s_songs[s_index].duration_sec) {
        s_pos = 0;
        publish_pos();
    }
    set_state(MUSIC_STATE_PLAYING);
}

void music_core_next(void)
{
    if (s_song_count == 0) return;
    int nxt;
    if (s_mode == MUSIC_MODE_SHUFFLE && s_song_count > 1) {
        nxt = random_index();
    } else {
        nxt = (s_index + 1 >= s_song_count) ? 0 : s_index + 1;
    }
    play_index_at(nxt);
}

void music_core_prev(void)
{
    if (s_song_count == 0) return;
    int prv;
    if (s_mode == MUSIC_MODE_SHUFFLE && s_song_count > 1) {
        prv = random_index();
    } else {
        prv = (s_index == 0) ? s_song_count - 1 : s_index - 1;
    }
    play_index_at(prv);
}

void music_core_cycle_mode(void)
{
    s_mode = (music_mode_t)((s_mode + 1) % 4);
    lv_subject_set_int(&g_music_mode_subject, (int)s_mode);
}

void music_core_set_volume(int v)
{
    if (v < 0) v = 0;
    else if (v > 100) v = 100;
    s_volume = (uint8_t)v;
    lv_subject_set_int(&g_music_volume_subject, v);
}

/* ---- 初始化 ---- */

void music_core_init(void)
{
    s_song_count = 0;
    s_index = 0;
    s_pos = 0;
    s_state = MUSIC_STATE_IDLE;
    s_mode = MUSIC_MODE_LOOP_ALL;
    s_volume = 60;
    s_title[0] = '\0';

    lv_subject_init_int(&g_music_state_subject, (int)s_state);
    lv_subject_init_int(&g_music_mode_subject, (int)s_mode);
    lv_subject_init_string(&g_music_title_subject, s_title, NULL,
        sizeof(s_title), "-");
    lv_subject_init_int(&g_music_pos_subject, 0);
    lv_subject_init_int(&g_music_duration_subject, 0);
    lv_subject_init_int(&g_music_index_subject, 0);
    lv_subject_init_int(&g_music_volume_subject, (int)s_volume);

    lv_timer_create(music_tick_cb, 1000, NULL);

    int n = scan_music_folder("playlist", s_songs, MUSIC_MAX_SONGS);
    if (n < 0) n = 0;
    s_song_count = (uint16_t)n;
    if (s_song_count > 0) goto_index(0);   /* 预置第一首,不自动播放 */
}

/* ---- 歌单扫描(自 ui/event/music.c 迁入) ---- */

static int is_audio_file(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;

    if (strcasecmp(ext, ".mp3") == 0) return 1;
    if (strcasecmp(ext, ".wav") == 0) return 1;
    if (strcasecmp(ext, ".flac") == 0) return 1;
    if (strcasecmp(ext, ".m4a") == 0) return 1;
    if (strcasecmp(ext, ".aac") == 0) return 1;
    return 0;
}

/* 模拟时长:真解码阶段由解码器回填真实值,此处先给确定性伪时长 */
static uint16_t fake_duration(const char *name)
{
    size_t h = 5381;
    const unsigned char *p = (const unsigned char *)name;
    while (*p) h = h * 33 + *p++;
    return (uint16_t)(120 + h % 181);   /* 120~300 秒 */
}

static int scan_music_folder(const char *dir_path, music_song_t *songs, int max_songs)
{
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("music: 打开歌单目录失败");
        return -1;
    }

    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL && count < max_songs) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (entry->d_type == DT_REG && is_audio_file(entry->d_name)) {
            snprintf(songs[count].path, MUSIC_SONG_PATH_LEN, "%s/%s",
                dir_path, entry->d_name);
            snprintf(songs[count].name, MUSIC_SONG_NAME_LEN, "%.*s",
                (int)MUSIC_SONG_NAME_LEN - 1, entry->d_name);
            songs[count].duration_sec = fake_duration(entry->d_name);
            count++;
        }
    }
    closedir(dir);
    return count;
}
