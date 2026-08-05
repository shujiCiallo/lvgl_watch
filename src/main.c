/**
 * @file main.c
 * 手表模拟器入口:初始化 LVGL/SDL 显示、数据层、UI 层,进入事件循环
 */

/*********************
 *      INCLUDES
 *********************/

#ifndef _DEFAULT_SOURCE
  #define _DEFAULT_SOURCE /* needed for usleep() */
#endif

#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
  #include <Windows.h>
#else
  #include <unistd.h>
  #include <pthread.h>
#endif
#include "lvgl/lvgl.h"
#include <SDL2/SDL.h>

#include "hal/hal.h"
#include "ui/ui.h"
#include "core/data_center.h"

#if LV_USE_OS != LV_OS_FREERTOS

int main(int argc, char **argv)
{
  (void)argc; /*Unused*/
  (void)argv; /*Unused*/

  /* 初始化 LVGL 核心 */
  lv_init();

  /* 初始化 SDL 显示与输入设备(HAL 层) */
  sdl_hal_init(320, 400);

  /* 初始化数据层(subject),必须在 UI 之前 */
  data_center_init();

  /* 初始化 UI(全局样式 + 各屏幕) */
  ui_init();

  /* 主循环:持续处理 LVGL 事件与重绘 */
  while(1) {
    /* 运行一次 LVGL 任务,返回下一次刷新前的可睡眠时间 */
    uint32_t sleep_time_ms = lv_timer_handler();
    if(sleep_time_ms == LV_NO_TIMER_READY){
	    sleep_time_ms =  LV_DEF_REFR_PERIOD;
    }
#ifdef _MSC_VER
    Sleep(sleep_time_ms);
#else
    usleep(sleep_time_ms * 1000);
#endif
  }

  return 0;
}

#endif
