#include "lvgl.h"
#include "music.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

music_panel_t music_panel;

static int scan_music_folder(const char *dir_path, song_entry_t *songs, int max_songs);
static lv_obj_t *music_label_create(lv_obj_t *parent);



void music_play_create(void)
{
    lv_obj_t *base = lv_obj_create(NULL);
    lv_obj_set_size(base, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(base, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(base, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START) ;

    music_panel.label = music_label_create(base);
    
    
}

static lv_obj_t *music_label_create(lv_obj_t *parent)
{
    lv_obj_t *label = lv_label_create(parent);
    lv_obj_set_size(label, lv_pct(100), lv_pct(30));
    
    lv_obj_t *label_music = lv_label_create(label);

    lv_obj_t *label_singer = lv_label_create(label);

    return label;
}

static song_entry_t s_songs[MAX_SONGS];

static void *music_get_list(void)
{
    int count = scan_music_folder("playlist", s_songs, MAX_SONGS);
    if (count <= 0) return NULL;

    return s_songs;
}

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

static int scan_music_folder(const char *dir_path, song_entry_t *songs, int max_songs) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("打开目录失败");
        return -1;
    }

    int count = 0;
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL && count < max_songs) {
        // 跳过 "." 和 ".." 目录
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // 只处理普通文件（且是音频格式）
        if (entry->d_type == DT_REG && is_audio_file(entry->d_name)) {
            // 拼接完整路径（假设路径以目录+文件名组成）
            snprintf(songs[count].path, MAX_PATH_LEN, "%s/%s", dir_path, entry->d_name);
            strncpy(songs[count].name, entry->d_name, sizeof(songs[count].name) - 1);
            count++;
        }
    }

    closedir(dir);
    return count;
}
