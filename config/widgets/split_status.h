#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/peripheral_status.h>
#include "../includes/split_battery_widget.dtsi"

#define CANVAS_SIZE 68
#define CANVAS_COLOR_FORMAT LV_COLOR_FORMAT_L8
#define CANVAS_BUF_SIZE LV_CANVAS_BUF_SIZE(CANVAS_SIZE, CANVAS_SIZE, LV_COLOR_FORMAT_GET_BPP(CANVAS_COLOR_FORMAT), LV_DRAW_BUF_STRIDE_ALIGN)

#define LVGL_BACKGROUND IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_INVERTED) ? lv_color_black() : lv_color_white()
#define LVGL_FOREGROUND IS_ENABLED(CONFIG_NICE_VIEW_WIDGET_INVERTED) ? lv_color_white() : lv_color_black()

struct zmk_widget_split_status {
    sys_snode_t node;
    lv_obj_t *obj;
    uint8_t cbuf[CANVAS_BUF_SIZE];
    struct {
        uint8_t central_level;
        uint8_t peripheral_level;
        bool peripheral_connected;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        bool usb_present;
#endif
    } state;
};

int zmk_widget_split_status_init(struct zmk_widget_split_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_split_status_obj(struct zmk_widget_split_status *widget);
