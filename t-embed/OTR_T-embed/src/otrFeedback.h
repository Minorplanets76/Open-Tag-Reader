// otrFeedbaack.h
#ifndef OTRFEEDBACK_H
#define OTRFEEDBACK_H

#define PIN_IIS_BCLK          7
#define PIN_IIS_WCLK          5
#define PIN_IIS_DOUT          6

#define PIN_APA102_CLK        45
#define PIN_APA102_DI         42

#define PIN_ENCODE_A          2
#define PIN_ENCODE_B          1
#define PIN_ENCODE_BTN        0

#define PIN_BAT_VOLT          4

#define PIN_VIBRATE          40
#define PIN_POWER_LED        38

#define LV_BUTTON bit(0)
#define LV_ENCODER_CW bit(1)
#define LV_ENCODER_CCW bit(2)
#define LV_ENCODER_LED_CW bit(3)
#define LV_ENCODER_LED_CCW bit(4)

#include "Audio.h"
#include "FS.h"
#include "LittleFS.h"
#include <Arduino.h>
#include "Wire.h"
#include "APA102.h" 
#include <OneButton.h> 
#include <RotaryEncoder.h>

static uint8_t LEDmode = 0;
static uint16_t newLEDmode = 0;

void initAudio(void);
void playScanGood(void);
void playScanBad(void);
void initLED(void);
void led_task(void *param);
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v);
void changeLEDmode(uint8_t LEDmode);
void initButton(void);
void button_pressed(void);
void getBatteryVoltage(void);
void getBatteryChargeState(void);

#endif