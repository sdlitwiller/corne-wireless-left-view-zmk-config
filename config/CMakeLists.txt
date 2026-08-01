if(CONFIG_ZMK_DISPLAY)
  target_sources_ifdef(CONFIG_NICE_VIEW_WIDGET_STATUS app PRIVATE custom_status_screen.c)
  target_sources_ifdef(CONFIG_NICE_VIEW_WIDGET_STATUS app PRIVATE widgets/split_status.c)
endif()
