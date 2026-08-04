#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "lvgl.h"

extern lv_obj_t *g_tileview;

typedef struct Button{
        lv_obj_t *btn;
        lv_obj_t *label;
        char *text;
}Button_t;
typedef struct data_label {
        lv_obj_t *label;
        void *data;
}data_label_t;

void screen_tileview_create(lv_obj_t *parent);

#endif
