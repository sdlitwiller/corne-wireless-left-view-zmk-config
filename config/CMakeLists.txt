if(CONFIG_ZMK_DISPLAY)
  target_sources(app PRIVATE custom_status_screen.c)
  target_sources(app PRIVATE widgets/lr_battery_widget.c)
endif()
