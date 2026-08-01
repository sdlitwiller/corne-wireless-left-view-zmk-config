#include "widgets/split_status.h"
#include <zephyr/kernel.h>

#if IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_STATUS)
static struct zmk_widget_split_status split_status_widget;
#endif

lv_obj_t *zmk_display_status_screen(void) {
  lv_obj_t *screen = lv_obj_create(NULL);

#if IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_STATUS)
  zmk_widget_split_status_init(&split_status_widget, screen);
  lv_obj_align(zmk_widget_split_status_obj(&split_status_widget),
               LV_ALIGN_TOP_LEFT, 0, 0);
#endif

  return screen;
}
