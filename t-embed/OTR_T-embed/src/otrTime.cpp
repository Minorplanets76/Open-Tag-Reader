#include "otrTime.h"


extern RTC_DS3231 rtc;
bool pmFlag;
int year;
int month;
int date;
int hour;
int minute;
int second;

void rtc_init() {
  
  Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);  //initialise RTC
  if (!rtc.begin())     {
    Serial.println("Couldn't find RTC");
    // return;
  }
  
  //
}

void print_local_time()   {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))   {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void set_RTC(int year, int month, int date, 
                  int hour, int minute, int second)   {
    rtc.adjust(DateTime(year, month, date, hour, minute, second));

}

void set_RTC_at_compile()   {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void timestamp(char timeStr[12], char dateStr[11]) {
    // Display RTC time and date
    DateTime now = rtc.now();

    if (now.isPM()) {
        sprintf(timeStr, "%02d:%02d:%02d PM", now.hour() - 12, now.minute(), now.second());
    } else {
        sprintf(timeStr, "%02d:%02d:%02d AM", now.hour(), now.minute(), now.second());
    }

    sprintf(dateStr, "%d/%d/%d", now.day(), now.month(), now.year());
}

void get_NTP_time() {
    configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))   {
        Serial.println("Failed to obtain time");
        return;
    }
    rtc.adjust(DateTime(timeinfo.tm_year+1900,timeinfo.tm_mon+1,timeinfo.tm_mday,
                        timeinfo.tm_hour, timeinfo.tm_min,timeinfo.tm_sec));
}

void get_RTC_temperature() {
    Serial.printf(" Temperature: %.1f `C\r\n", rtc.getTemperature());
}
