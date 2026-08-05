#include "hal.h"

/* SDL HAL 初始化:创建显示、鼠标、滚轮、键盘输入设备,并挂上光标 */
lv_display_t * sdl_hal_init(int32_t w, int32_t h)
{
  lv_group_set_default(lv_group_create());

  /* 创建 SDL 模拟显示屏 */
  lv_display_t * disp = lv_sdl_window_create(w, h);

  /* 鼠标输入:绑定到默认组,并用光标图标 */
  lv_indev_t * mouse = lv_sdl_mouse_create();
  lv_indev_set_group(mouse, lv_group_get_default());
  lv_indev_set_display(mouse, disp);
  lv_display_set_default(disp);

  /* 声明并创建鼠标光标对象 */
  LV_IMAGE_DECLARE(mouse_cursor_icon);
  lv_obj_t * cursor_obj;
  cursor_obj = lv_image_create(lv_screen_active());
  lv_image_set_src(cursor_obj, &mouse_cursor_icon);
  lv_indev_set_cursor(mouse, cursor_obj);

  /* 鼠标滚轮输入 */
  lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
  lv_indev_set_display(mousewheel, disp);
  lv_indev_set_group(mousewheel, lv_group_get_default());

  /* 键盘输入 */
  lv_indev_t * kb = lv_sdl_keyboard_create();
  lv_indev_set_display(kb, disp);
  lv_indev_set_group(kb, lv_group_get_default());

  return disp;
}
