// otrDisplay.h
#ifndef OTRDISPLAY_H
#define OTRDISPLAY_H

#include <Arduino.h>
#include "TFT_eSPI.h"
#include <lvgl.h>
#include "otrFeedback.h"
#include "otrData.h"

#define PIN_LCD_BL            15
#define PIN_LCD_DC            13
#define PIN_LCD_CS            10
#define PIN_LCD_CLK           12
#define PIN_LCD_MOSI          11
#define PIN_LCD_RES           9
#define PIN_LCD_POWER_ON      46

#define LV_SCREEN_WIDTH       170
#define LV_SCREEN_HEIGHT      320
#define LV_BUF_SIZE           (LV_SCREEN_WIDTH * LV_SCREEN_HEIGHT)

void initDisplay(void);
void lv_begin();
void lv_handler();
void lv_example_btn_1(void);
void printProgressDot();

#endif