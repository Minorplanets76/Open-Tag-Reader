// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: SquareLine_Project_Example_UI

#include "ui.h"
#include "ui_events.h"
#include <Arduino.h>
#include "otrTime.h"
#include "otrData.h"
#include "otrFeedback.h"
#include "otrScanning.h"

extern RTC_DS3231 rtc;
extern VIBRATE vibrate;
extern RFIDReader reader;
extern LED led;
extern int ledPin;
extern int buzzerPin;
extern void playNote(int buzzerPin);
extern RECORDS records;
extern Species species;
void btn_event_cb(lv_event_t * e)
{
	    static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        //lv_label_set_text_fmt(ui_Main_Label2, "Button: %d", cnt);
        // toggle the LED when the button is pressed
        //toggleLed(ledPin);
        // playNote(buzzerPin);
        // vibrate.tap();
        // led.toggle();
}

void setTimeManual(lv_event_t * e)
{
	// Your code here
        int hour = lv_roller_get_selected(ui_Time_RollerHour) + 1;
        int minute = lv_roller_get_selected(ui_Time_RollerMinute);
        int AMPM = lv_roller_get_selected(ui_Time_RollerAMPM);
        if (AMPM != 0) {
            hour = hour + 12;
        }
        DateTime now = rtc.now();
        rtc.adjust(DateTime(now.year(), now.month(), now.day(), hour, minute, 0));

        
}

void setDateManual(lv_event_t * e)
{
        int day = lv_roller_get_selected(ui_Date_RollerDay) +1;
        int month = lv_roller_get_selected(ui_Date_RollerMonth) +1;
        int year = lv_roller_get_selected(ui_Date_RollerYear)+2019;

        DateTime now = rtc.now();
        rtc.adjust(DateTime(year, month, day, now.hour(), now.minute(), now.second()));
	
}

void UI_MAIN_SCREEN_LOAD(lv_event_t * e)
{
	// updateTimeToScreen();
        // updateBatteryImage();
        RECORDS data;
        lv_roller_set_options(ui_Main_RollerRecentSessions, data.readLastSessions().c_str(), LV_ROLLER_MODE_NORMAL);
        _ui_roller_set_property(ui_Main_RollerRecentSessions, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, 4);
        Serial.print("From main screen load: ");
        Serial.print(data.readLastSessions().c_str());
        Serial.print("end");
        

}



void scanSwitched(lv_event_t * e)
{
	reader.toggle();
}

void timeSetScreenLoad(lv_event_t * e)
{
	//Set Rollers to current time
        DateTime now = rtc.now();
        if (now.hour() > 12) {
            _ui_roller_set_property(ui_Time_RollerHour, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.hour()-13);
            _ui_roller_set_property(ui_Time_RollerAMPM, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, 1);
        }
        else {
            _ui_roller_set_property(ui_Time_RollerHour, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.hour()-1);
            _ui_roller_set_property(ui_Time_RollerAMPM, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, 0);
        }
        
        _ui_roller_set_property(ui_Time_RollerMinute, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.minute());
        _ui_roller_set_property(ui_Date_RollerDay, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.day()-1);
        _ui_roller_set_property(ui_Date_RollerMonth, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.month()-1);
        _ui_roller_set_property(ui_Date_RollerYear, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.year()-2019);


}






void setTimeTabChange(lv_event_t * e)
{
	DateTime now = rtc.now();
        if(lv_tabview_get_tab_act(ui_SetTime_TabView)) {
                //date screen
                _ui_roller_set_property(ui_Time_RollerMinute, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.minute());
                _ui_roller_set_property(ui_Date_RollerDay, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.day()-1);
                _ui_roller_set_property(ui_Date_RollerMonth, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.month()-1);
                _ui_roller_set_property(ui_Date_RollerYear, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.year()-2019);
        }
        else {
                //time screen
                if (now.hour() > 12) {
                _ui_roller_set_property(ui_Time_RollerHour, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.hour()-13);
                _ui_roller_set_property(ui_Time_RollerAMPM, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, 1);
                }
                else {
                _ui_roller_set_property(ui_Time_RollerHour, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.hour()-1);
                _ui_roller_set_property(ui_Time_RollerAMPM, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, 0);
                }
                _ui_roller_set_property(ui_Time_RollerMinute, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.minute());
        }
       
}
void timeScreenLoaded(lv_event_t * e)
{
    char timeClock[5];
    DateTime now = rtc.now();
    if (now.isPM()) {
        sprintf(timeClock, "%d:%02d", now.hour() - 12, now.minute());
        lv_label_set_text_fmt(ui_Time_Label_Time, "%s PM", timeClock);
    } else {
        sprintf(timeClock, "%d:%02d", now.hour(), now.minute());
        lv_label_set_text_fmt(ui_Time_Label_Time, "%s AM", timeClock);
    }
    updateDateToScreen();
}

void SetTimeScreenLoaded(lv_event_t * e)
{
	DateTime now = rtc.now();
        if (now.hour() > 12) {
            _ui_roller_set_property(ui_Time_RollerHour, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.hour()-13);
            _ui_roller_set_property(ui_Time_RollerAMPM, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, 1);
        }
        else {
            _ui_roller_set_property(ui_Time_RollerHour, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.hour()-1);
            _ui_roller_set_property(ui_Time_RollerAMPM, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, 0);
        }
        _ui_roller_set_property(ui_Time_RollerMinute, _UI_ROLLER_PROPERTY_SELECTED_WITH_ANIM, now.minute());
}

void newSession(lv_event_t * e)
{
        //copy last session to records 
	_ui_screen_change(&ui_Scan, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, &ui_Scan_screen_init);
        records.lastSessionFilePath = "/" + speciesStrings[species] + "/sessions/last_sessions.txt";
        records.createSession();
}

void continueSession(lv_event_t * e)
{
        records.recordsCounted = false;

        _ui_screen_change(&ui_Scan, LV_SCR_LOAD_ANIM_FADE_ON, 200, 0, &ui_Scan_screen_init);

}
 