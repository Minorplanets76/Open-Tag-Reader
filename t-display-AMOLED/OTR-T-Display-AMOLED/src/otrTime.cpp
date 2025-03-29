#include "otrTime.h"
#include <LV_Helper.h>
#include <lvgl.h>
#include "ui/ui.h"

extern RTC_DS3231 rtc;
bool pmFlag;
int year;
int month;
int date;
int hour;
int minute;
int second;

void rtc_init() {
  
  //Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);  //initialise RTC

  if (!rtc.begin())     {
    Serial.println("Couldn't find RTC");
    // return;
  }
    if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  }

}


void set_RTC(int year, int month, int date, 
                  int hour, int minute, int second)   {
    rtc.adjust(DateTime(year, month, date, hour, minute, second));

}

void set_RTC_at_compile()   {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void timeStamp(char timeStr[12], char dateStr[11]) {
    // Display RTC time and date
    DateTime now = rtc.now();

    if (now.isPM()) {
        sprintf(timeStr, "%02d:%02d:%02d PM", now.hour() - 12, now.minute(), now.second());
    } else {
        sprintf(timeStr, "%02d:%02d:%02d AM", now.hour(), now.minute(), now.second());
    }

    sprintf(dateStr, "%d/%d/%d", now.day(), now.month(), now.year());
}

void updateTimeToScreen()   {
    char timeClock[5];
    DateTime now = rtc.now();
    if (now.isPM()) {
        sprintf(timeClock, "%d:%02d", now.hour() - 12, now.minute());
    } else {
        sprintf(timeClock, "%d:%02d", now.hour(), now.minute());
    }
    lv_label_set_text_fmt(ui_Main_TopPanelTime, "%s", timeClock);
    lv_label_set_text_fmt(ui_Scan_TopPanelTime, "%s", timeClock);
  
    
}

void updateDateToScreen()   {
    char dateStr[11];
    DateTime now = rtc.now();
    sprintf(dateStr, "%d/%d/%d", now.day(), now.month(), now.year());
    Serial.println(dateStr);
    lv_label_set_text_fmt(ui_Time_Label_Date, "%s", dateStr);
}

void get_RTC_temperature() {
    Serial.printf(" Temperature: %.1f °C\r\n", rtc.getTemperature());
}
void clock_timer(lv_timer_t * timer) {
    updateTimeToScreen();
    updateBatteryImage();
}
DateTime stringToDateTime(String dateStr) {
    int day = dateStr.substring(0, dateStr.indexOf('/')).toInt();
    int month = dateStr.substring(dateStr.indexOf('/') + 1, dateStr.lastIndexOf('/')).toInt();
    int year = dateStr.substring(dateStr.lastIndexOf('/') + 1).toInt();
    return DateTime(year, month, day);
}
String dateTimeToString(DateTime dt) {
    char dateStr[11];
    sprintf(dateStr, "%02d/%02d/%04d", dt.day(), dt.month(), dt.year());
    return String(dateStr);
}

String timeStampToString(DateTime dt) {
    char dateStr[20];
    sprintf(dateStr, "%02d/%02d/%04d %02d:%02d:%02d", dt.day(), dt.month(), dt.year(), dt.hour(), dt.minute(), dt.second());
    return String(dateStr);
}

DateTime stringToTimeStamp(String tmStmp) {
    int day = tmStmp.substring(0, tmStmp.indexOf('/')).toInt();
    int month = tmStmp.substring(tmStmp.indexOf('/') + 1, tmStmp.lastIndexOf('/')).toInt();
    int year= tmStmp.substring(tmStmp.lastIndexOf('/') + 1, 4).toInt();
    int hour = tmStmp.substring(tmStmp.indexOf(' ') + 1, tmStmp.indexOf(':')).toInt();
    int minute = tmStmp.substring(tmStmp.indexOf(':') + 1, tmStmp.lastIndexOf(':')).toInt();
    int second = tmStmp.substring(tmStmp.lastIndexOf(':') + 1).toInt();
    return DateTime(year, month, day, hour, minute, second);
}

String dateToSessionFormat(DateTime dt) {
    char datestr[10];
    int year = dt.year()-2000;
    sprintf(datestr, "%02d%02d%02d", year, dt.month(), dt.day());
    Serial.println(datestr);
    return String(datestr);
}

String ageFromDate(String dateStr) {
    DateTime dt = stringToDateTime(dateStr);
    DateTime now = rtc.now();
    int ageInMonths = (now.year() - dt.year()) * 12 + (now.month() - dt.month());
    if (ageInMonths < 18) {
        return String(ageInMonths) + " mths";
    } else {
        float ageInYears = ageInMonths / 12;
        return String(ageInYears, 1) + " yrs ";
    }
}

String whpSafeDate(String dateStr, String whp) {
    DateTime dt = stringToDateTime(dateStr);
    int daysToAdd = whp.toInt();
        
    DateTime newDt = dt + TimeSpan(daysToAdd, 0, 0, 0);
    return dateTimeToString(newDt);
}