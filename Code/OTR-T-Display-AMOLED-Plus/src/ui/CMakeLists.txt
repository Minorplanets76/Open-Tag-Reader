SET(SOURCES screens/ui_Main.c
    screens/ui_Time.c
    screens/ui_SetTime.c
    screens/ui_Settings.c
    screens/ui_Scan.c
    screens/ui_Records.c
    screens/ui_New.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.cpp
    images/ui_img_battery_3_bar_png.c
    images/ui_img_contactless_png.c
    images/ui_img_settings_png.c
    images/ui_img_contactless_off_png.c
    images/ui_img_wifi_tethering_off_png.c
    images/ui_img_wifi_tethering_png.c
    images/ui_img_wifi_off_png.c
    images/ui_img_wifi_png.c
    images/ui_img_bluetooth_disabled_png.c
    images/ui_img_bluetooth_png.c
    images/ui_img_volume_up_png.c
    images/ui_img_add_circle_png.c
    images/ui_img_arrow_back_png.c
    images/ui_img_battery_1_bar_png.c
    images/ui_img_battery_2_bar_png.c
    images/ui_img_battery_4_bar_png.c
    images/ui_img_battery_5_bar_png.c
    images/ui_img_battery_alert_png.c
    images/ui_img_battery_charging_full_png.c
    images/ui_img_battery_full_png.c
    images/ui_img_cancel_png.c
    images/ui_img_sd_card_png.c
    images/ui_img_volume_down_png.c
    images/ui_img_volume_off_png.c)

add_library(ui ${SOURCES})
