SET(SOURCES screens/ui_Main.c
    screens/ui_Time.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.cpp
    images/ui_img_battery_full_png.c
    images/ui_img_contactless_png.c
    images/ui_img_settings_png.c
    images/ui_img_contactless_off_png.c)

add_library(ui ${SOURCES})
