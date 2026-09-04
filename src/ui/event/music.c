#include "lvgl.h"
#include "music.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include "widgets/ui_widgets.h"

static int scan_music_folder(const char *dir_path, song_entry_t *songs, int max_songs);
static lv_obj_t *music_label_create(lv_obj_t *parent);
static lv_obj_t *music_ctl_create(lv_obj_t *parent);
static lv_obj_t *music_tools_create(lv_obj_t *parent);
static void btn_create(lv_obj_t *parent);

music_state_t music_state = PAUSED;
play_mode_t play_mode;

void music_panel_create(music_panel_t *self)
{
    self->root = lv_obj_create(NULL);
    lv_obj_set_size(self->root, lv_pct(100), lv_pct(100));
    lv_obj_set_flex_flow(self->root, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(self->root, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START) ;

    self->label = music_label_create(self->root);
    self->btn = music_ctl_create(self->root);
    self->tools = music_tools_create(self->root);
    
}

static lv_obj_t *music_label_create(lv_obj_t *parent)
{
    lv_obj_t *label = lv_obj_create(parent);
    lv_obj_set_width(label, lv_pct(100));
    lv_obj_set_flex_grow(label, 3);
    lv_obj_set_flex_flow(label, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(label, LV_FLEX_ALIGN_START,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    
    lv_obj_t *label_music = lv_label_create(label);
    lv_obj_set_style_text_font(label_music, &lv_font_montserrat_36, 0);

    return label;
}

static lv_obj_t *music_ctl_create(lv_obj_t *parent)
{
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_set_width(obj, lv_pct(100));
    lv_obj_set_flex_grow(obj, 6);
    // lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);
    // lv_obj_set_flex_align(obj, LV_FLEX_ALIGN_SPACE_EVENLY,
    //     LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);


    lv_obj_t *arc = lv_arc_create(obj);
    // lv_coord_t arc_size_h = lv_obj_get_width(obj);
    lv_coord_t arc_size_w = lv_obj_get_width(obj);
    // lv_obj_set_size(arc, arc_size_w/2, arc_size_w/2);
    lv_obj_center(arc);
    lv_obj_set_style_opa(arc, LV_OPA_TRANSP, LV_PART_KNOB);
    lv_arc_set_bg_angles(arc, 0, 360);
    lv_arc_set_angles(arc, 0, 360);
    lv_arc_set_value(arc, 0);
    // lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    // lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_clickable(arc, false);

    btn_create(obj);

    return obj;
}

static void prev_click_cb(lv_event_t *e);
static void toggle_click_cb(lv_event_t *e);
static void next_click_cb(lv_event_t *e);

static void btn_create(lv_obj_t *parent)
{
    lv_obj_t *obj = parent;
    static lv_style_t music_ctl_style;

    lv_style_init(&music_ctl_style);
    lv_style_set_text_font(&music_ctl_style, &lv_font_montserrat_30);
    lv_style_set_text_align(&music_ctl_style, LV_TEXT_ALIGN_CENTER);
    // lv_style_set_bg_color(&music_ctl_style, 
    //     lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_opa(&music_ctl_style, LV_OPA_TRANSP);


    lv_obj_t *btn = lv_obj_create(obj);
    lv_obj_remove_style_all(btn);
    lv_obj_set_width(btn, lv_pct(100));
    lv_obj_set_height(btn, LV_SIZE_CONTENT);
    lv_obj_center(btn);
    lv_obj_set_flex_flow(btn, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn, LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);

    lv_obj_t *prev = lv_label_create(btn);
    lv_obj_add_style(prev, &music_ctl_style, 0);
    lv_obj_set_flex_grow(prev, 3);
    lv_label_set_text(prev, LV_SYMBOL_PREV);
    lv_obj_set_size(prev, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_size(prev, lv_pct(100), lv_pct(100));

    lv_obj_t *toggle = lv_label_create(btn);
    lv_obj_add_style(toggle, &music_ctl_style, 0);
    lv_obj_set_flex_grow(toggle, 5);
    lv_obj_set_style_text_font(toggle, &lv_font_montserrat_40, 0);
    lv_label_set_text(toggle, LV_SYMBOL_PLAY);
    lv_obj_set_size(toggle, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_size(toggle, lv_pct(100), lv_pct(100));
    lv_obj_add_flag(toggle, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(toggle, toggle_click_cb, 
        LV_EVENT_CLICKED, &music_state);

    lv_obj_t *next = lv_label_create(btn);
    lv_obj_add_style(next, &music_ctl_style, 0);
    lv_obj_set_flex_grow(next, 3);
    lv_label_set_text(next, LV_SYMBOL_NEXT);
    lv_obj_set_size(next, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    // lv_obj_set_size(next, lv_pct(100), lv_pct(100));

}

static lv_obj_t *music_tools_create(lv_obj_t *parent)
{
    lv_obj_t *tools = lv_obj_create(parent);
    lv_obj_set_width(tools, lv_pct(100));
    lv_obj_set_flex_grow(tools, 4);
    lv_obj_set_flex_flow(tools, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(tools, LV_FLEX_ALIGN_SPACE_EVENLY,
        LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);

    lv_obj_t *volume = lv_label_create(tools);
    lv_obj_set_height(volume, lv_pct(100));
    lv_obj_set_flex_grow(volume, 2);
    lv_obj_set_style_text_font(volume, &lv_font_montserrat_20, 0);

    lv_obj_t *mode = lv_label_create(tools);
    lv_obj_set_height(volume, lv_pct(100));
    lv_obj_set_flex_grow(mode, 2);
    lv_obj_set_style_text_font(mode, &lv_font_montserrat_20, 0);
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

static void prev_click_cb(lv_event_t *e)
{

}
static void toggle_click_cb(lv_event_t *e)
{
    // printf("toggle_click_cb triggered!\n");
    music_state_t *state = lv_event_get_user_data(e);
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        if (*state == PLAYING) {
            lv_label_set_text(btn, LV_SYMBOL_PAUSE);
            music_state = PAUSED;
        } else if (*state == PAUSED) {
            lv_label_set_text(btn, LV_SYMBOL_PLAY);
            music_state = PLAYING;
        }

    }

}
static void next_click_cb(lv_event_t *e)
{

}