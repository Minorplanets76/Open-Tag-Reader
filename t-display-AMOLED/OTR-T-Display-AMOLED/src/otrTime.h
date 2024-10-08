// otrTime.h
#ifndef OTRTIME_H
#define OTRTIME_H

#include <Arduino.h>
#include "time.h"
#include "Wire.h"
#include "RTClib.h"
#include "otrFeedback.h"


#define PIN_IIC_SDA           3
#define PIN_IIC_SCL           2

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
void timeStamp(char timeStr[12], char dateStr[11]);
void updateTimeToScreen(void);
void updateDateToScreen(void);
void get_RTC_temperature(void);
void clock_timer(lv_timer_t * timer);
DateTime stringToDate(String dateStr);
String dateToString(DateTime dt);
String timeStampToString(DateTime dt);
DateTime stringToTimeStamp(String tmStmp);
String dateToSessionFormat(DateTime dt);
String ageFromDate(String dateStr);
String whpSafeDate(String dateStr, String whp);

#endif