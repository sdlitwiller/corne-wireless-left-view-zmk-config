#include "widgets/lr_battery_widget.c"
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zmk/display.h>

lv_obj_t *zmk_display_status_screen(void) {
  lv_obj_t *screen = lv_obj_create(NULL);
  zmk_widget_lr_battery_init(screen);
  lv_obj_align(zmk_widget_lr_battery_obj(), LV_ALIGN_TOP_LEFT, 0, 0);
  return screen;
}
