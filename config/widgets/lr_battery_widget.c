#include <zephyr/kernel.h>

#include <lvgl.h>
#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/usb.h>

struct lr_battery_state {
  uint8_t left_level;
  uint8_t right_level;
};

struct lr_battery_widget {
  lv_obj_t *obj;
  struct lr_battery_state state;
};

static struct lr_battery_widget widget;

static void draw_widget(struct lr_battery_widget *w) {
  char text[32] = {};
  lv_obj_clean(w->obj);

  snprintf(text, sizeof(text), "L: %u%%\nR: %u%%", w->state.left_level,
           w->state.right_level);
  lv_obj_t *label = lv_label_create(w->obj);
  lv_label_set_text(label, text);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

static void update_from_battery(uint8_t level) {
  widget.state.left_level = level;
  widget.state.right_level = level;
  draw_widget(&widget);
}

static void battery_status_update_cb(struct battery_status_state state) {
  update_from_battery(state.level);
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

int zmk_widget_lr_battery_init(lv_obj_t *parent) {
  widget.obj = lv_obj_create(parent);
  lv_obj_set_size(widget.obj, 160, 68);
  widget.state.left_level = 0;
  widget.state.right_level = 0;
  draw_widget(&widget);
  return 0;
}

lv_obj_t *zmk_widget_lr_battery_obj(void) { return widget.obj; }
