#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/split_peripheral_status_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/split/bluetooth/central.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/usb.h>

#include "split_status.h"

struct split_status_state {
  uint8_t central_level;
  uint8_t peripheral_level;
  bool peripheral_connected;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
  bool usb_present;
#endif
};

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static void draw_battery_segment(lv_obj_t *canvas, int x, int y,
                                 uint8_t level) {
  lv_draw_rect_dsc_t rect_black_dsc;
  lv_draw_rect_dsc_t rect_white_dsc;

  init_rect_dsc(&rect_black_dsc, LVGL_BACKGROUND);
  init_rect_dsc(&rect_white_dsc, LVGL_FOREGROUND);

  canvas_draw_rect(canvas, x, y, 24, 10, &rect_black_dsc);
  canvas_draw_rect(canvas, x + 1, y + 1, 22, 8, &rect_white_dsc);
  canvas_draw_rect(canvas, x + 2, y + 2, (level + 2) / 4, 6, &rect_black_dsc);
  canvas_draw_rect(canvas, x + 25, y + 3, 2, 4, &rect_white_dsc);
}

static void draw_screen(lv_obj_t *widget,
                        const struct split_status_state *state) {
  lv_obj_t *canvas = lv_obj_get_child(widget, 0);
  lv_canvas_fill_bg(canvas, LVGL_BACKGROUND, LV_OPA_COVER);

  char text[16] = {};
  snprintf(text, sizeof(text), "L:%3u%%", state->central_level);
  lv_draw_label_dsc_t label_dsc;
  init_label_dsc(&label_dsc, LVGL_FOREGROUND, &lv_font_montserrat_14,
                 LV_TEXT_ALIGN_LEFT);
  canvas_draw_text(canvas, 0, 0, 80, &label_dsc, text);

  snprintf(text, sizeof(text), "R:%3u%%", state->peripheral_level);
  canvas_draw_text(canvas, 0, 18, 80, &label_dsc, text);

  if (state->peripheral_connected) {
    canvas_draw_text(canvas, 0, 36, 80, &label_dsc, LV_SYMBOL_WIFI);
  } else {
    canvas_draw_text(canvas, 0, 36, 80, &label_dsc, LV_SYMBOL_CLOSE);
  }

  draw_battery_segment(canvas, 70, 0, state->central_level);
  draw_battery_segment(canvas, 70, 18, state->peripheral_level);

  rotate_canvas(canvas);
}

static void set_status(struct zmk_widget_split_status *widget,
                       struct split_status_state state) {
  widget->state.central_level = state.central_level;
  widget->state.peripheral_level = state.peripheral_level;
  widget->state.peripheral_connected = state.peripheral_connected;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
  widget->state.usb_present = state.usb_present;
#endif
  draw_screen(widget->obj, &widget->state);
}

static void battery_status_update_cb(struct battery_status_state state) {
  struct zmk_widget_split_status *widget;
  SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
    set_status(widget,
               (struct split_status_state){
                   .central_level = state.level,
                   .peripheral_level = widget->state.peripheral_level,
                   .peripheral_connected = widget->state.peripheral_connected,
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
                   .usb_present = state.usb_present,
#endif
               });
  }
}

static struct battery_status_state
battery_status_get_state(const zmk_event_t *eh) {
  return (struct battery_status_state){
      .level = zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
      .usb_present = zmk_usb_is_powered(),
#endif
  };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif

static void peripheral_status_update_cb(struct peripheral_status_state state) {
  struct zmk_widget_split_status *widget;
  SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
    set_status(widget, (struct split_status_state){
                           .central_level = widget->state.central_level,
                           .peripheral_level = widget->state.peripheral_level,
                           .peripheral_connected = state.connected,
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
                           .usb_present = widget->state.usb_present,
#endif
                       });
  }
}

static struct peripheral_status_state
peripheral_status_get_state(const zmk_event_t *_eh) {
  return (struct peripheral_status_state){
      .connected = zmk_split_bt_peripheral_is_connected()};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_status,
                            struct peripheral_status_state,
                            peripheral_status_update_cb,
                            peripheral_status_get_state)
ZMK_SUBSCRIPTION(widget_peripheral_status, zmk_split_peripheral_status_changed);

int zmk_widget_split_status_init(struct zmk_widget_split_status *widget,
                                 lv_obj_t *parent) {
  widget->obj = lv_obj_create(parent);
  lv_obj_set_size(widget->obj, 160, 68);
  lv_obj_t *canvas = lv_canvas_create(widget->obj);
  lv_obj_align(canvas, LV_ALIGN_TOP_LEFT, 0, 0);
  lv_canvas_set_buffer(canvas, widget->cbuf, CANVAS_SIZE, CANVAS_SIZE,
                       CANVAS_COLOR_FORMAT);

  sys_slist_append(&widgets, &widget->node);
  widget_battery_status_init();
  widget_peripheral_status_init();
  return 0;
}

lv_obj_t *zmk_widget_split_status_obj(struct zmk_widget_split_status *widget) {
  return widget->obj;
}
