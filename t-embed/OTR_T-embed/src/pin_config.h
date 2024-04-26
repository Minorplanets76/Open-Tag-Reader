#pragma once



// #define WIFI_SSID             "Your-ssid"
// #define WIFI_PASSWORD         "Your-password"

#define WIFI_CONNECT_WAIT_MAX (30 * 1000)



#define LV_SCREEN_WIDTH       320
#define LV_SCREEN_HEIGHT      170
#define LV_BUF_SIZE           (LV_SCREEN_WIDTH * LV_SCREEN_HEIGHT)
/*ESP32S3*/
#define PIN_POWER_ON          46





#define PIN_LCD_BL            15
#define PIN_LCD_DC            13
#define PIN_LCD_CS            10
#define PIN_LCD_CLK           12
#define PIN_LCD_MOSI          11
#define PIN_LCD_RES           9





#define PIN_ES7210_BCLK       47
#define PIN_ES7210_LRCK       21
#define PIN_ES7210_DIN        14
#define PIN_ES7210_MCLK       48



#define RADIO_CS_PIN          17
#define RADIO_SW1_PIN         43
#define RADIO_SW0_PIN         44
#define NFC_CS                16