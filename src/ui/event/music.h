#ifndef MUSIC_H
#define MUSIC_H

#include "lvgl.h"

#define MAX_PATH_LEN    512
#define MAX_SONGS       100

typedef struct {
    char path[MAX_PATH_LEN];
    char name[128];
}song_entry_t;

typedef enum {
    IDLE,
    PLAYING,
    PAUSED,
    STOPPED,
    ERROR
}music_state_t;

typedef enum {
    SEQUENTIAL,
    LOOP_ALL,
    LOOP_ONE,
    SHUFFLE,
    BROWSE
}palymode_t;

typedef enum {
    INIT_OK,
    FINISH,
    BUFFER_UNDERRUN,
    DECODER_ERROR,
    FILE_EOF
}decoder_event_t;

typedef struct {
    lv_obj_t *root;
    lv_obj_t *label;
    lv_obj_t *btn;
    lv_obj_t *tools;
    void *list;
}music_panel_t;

typedef struct {
    music_state_t state;
    palymode_t mode;
    uint16_t total_duration;
    uint16_t current_pos;
}play_context_t;

#endif