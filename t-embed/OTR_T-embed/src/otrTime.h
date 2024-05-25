// otrTime.h
#ifndef OTRTIME_H
#define OTRTIME_H

#include <Arduino.h>
#include "time.h"
#include "Wire.h"
#include "RTClib.h"
#include "otrDisplay.h"

#define PIN_IIC_SDA           18
#define PIN_IIC_SCL           8

#define NTP_SERVER1           "au.pool.ntp.org"
#define NTP_SERVER2           "pool.ntp.org"
#define GMT_OFFSET_SEC        (3600 * 10)
#define DAY_LIGHT_OFFSET_SEC  3600

extern bool pmFlag;
extern int year;
extern int month;
extern int date;
extern int hour;
extern int minute;
extern int second;

void set_RTC(int year, int month, int date,
                  int hour, int minute, int second);

void print_local_time(void);
void rtc_init(void);
void set_RTC_at_compile(void);
void timestamp(void);


#endif