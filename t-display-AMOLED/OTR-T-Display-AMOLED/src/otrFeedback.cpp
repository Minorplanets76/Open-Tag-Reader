#include "otrFeedback.h"
#include <map>
#include <string>
#include <vector>
#include <esp_adc_cal.h>
#include <driver/gpio.h>
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <lvgl.h>
#include "ui/ui.h"

extern LilyGo_Class amoled;
extern LED led;
int buzzTime;
int startBuzzTime;

float readVbusVoltage() {
            esp_adc_cal_characteristics_t adc_chars;
            esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_12, ADC_WIDTH_BIT_12, 1100, &adc_chars);
            uint32_t v2 = 0,  raw = 0;
            raw = analogRead(vbusPin);
            //Serial.println(raw);
            v2 = esp_adc_cal_raw_to_voltage(raw, &adc_chars) * 2;
            //Serial.println(v2);
            return v2;
    
}
uint16_t batteryVoltage() {
    uint16_t batteryVoltageMV = amoled.getBattVoltage();
    return batteryVoltageMV;
}


extern const lv_img_dsc_t ui_img_battery_full_png;
extern const lv_img_dsc_t ui_img_battery_5_bar_png;
extern const lv_img_dsc_t ui_img_battery_4_bar_png;
extern const lv_img_dsc_t ui_img_battery_3_bar_png;
extern const lv_img_dsc_t ui_img_battery_2_bar_png;
extern const lv_img_dsc_t ui_img_battery_1_bar_png;
extern const lv_img_dsc_t ui_img_battery_alert_png;
extern const lv_img_dsc_t ui_img_battery_charging_full_png;

void updateBatteryImage() {
    uint16_t voltage = batteryVoltage();
    uint16_t vbus = readVbusVoltage();


    const lv_img_dsc_t* image;


    if (voltage >= 4150 && voltage <= 4250) {
        image = &ui_img_battery_full_png;
    } else if (voltage >= 4020 && voltage < 4150) {
        image = &ui_img_battery_5_bar_png;
    } else if (voltage >= 3870 && voltage < 4020) {
        image = &ui_img_battery_4_bar_png;
    } else if (voltage >= 3800 && voltage < 3870) {
        image = &ui_img_battery_3_bar_png;
    } else if (voltage >= 3730 && voltage < 3800) {
        image = &ui_img_battery_2_bar_png;
    } else if (voltage >= 3690 && voltage < 3730) {
        image = &ui_img_battery_1_bar_png;
    } else {
        image = &ui_img_battery_alert_png;
    }
    if (vbus >= 4000) { 
        image = &ui_img_battery_charging_full_png;
    }
    lv_img_set_src(ui_Main_TopPanelBattery, image);
    lv_img_set_src(ui_Scan_TopPanelBattery, image);
    
}

// Define a map to store note frequencies based on note names
std::map<std::string, int> noteFrequencies = {
    {"C4", 261},
    {"C#4", 277},
    {"D4", 294},
    {"D#4", 311},
    {"E4", 330},
    {"F4", 349},
    {"F#4", 370},
    {"G4", 392},
    {"G#4", 415},
    {"A4", 440},
    {"A#4", 466},
    {"B4", 494},
    {"C5", 523},
    {"C#5", 554},
    {"D5", 587},
    {"D#5", 622},
    {"E5", 659},
    {"F5", 698},
    {"F#5", 740},
    {"G5", 784},
    {"G#5", 831},
    {"A5", 880},
    {"A#5", 932},
    {"B5", 988}
};


// Function to play the melody "Click Goes The Shears"
#include <vector>

// Function to play the melody "Click Goes The Shears"
void playClickGoesTheShears(int buzzerPin)
{
    // Melody notes and pauses defined by their names ("P" for pause)
    std::vector<std::string> melody = {"D5", "P", "D5", "C5", "B4", "G4", "C5", "E5", "C5"};
    
    // Corresponding note durations in milliseconds
    const int noteDurations[] = {600, 10, 400, 200, 600, 600, 600, 600, 1000}; // Adjust durations as needed
    
    const int noteDuration = 600; // Default note duration in milliseconds
    
    for(int i = 0; i < melody.size(); i++) {
        if (melody[i] == "P") {
            // Pause (do nothing for rest duration)
            delay(noteDurations[i]);
        } else {
            int frequency = noteFrequencies[melody[i]]; // Assuming noteFrequencies map is defined elsewhere
            tone(buzzerPin, frequency, noteDurations[i]);
            
        }
    }
}

// Function to play the melody "Waltzing Matilda" using note names
void playWaltzingMatilda(int buzzerPin)
{
    // Melody notes and pauses defined by their names ("P" for pause)
    std::vector<std::string> melody = {
        "D5", "P", "D5", "P", "D5", "P", "D5", "B4", "G5", "P", "G5", "P", "G5", "F#5", "E5",
        "D5", "P", "D5", "P", "D5", "E5", "D5", "P", "D5", "P", "D5", "C5", "B4", "A4"
    };
    
    // Corresponding note durations in milliseconds
    const int noteDurations[] = {
        400, 10, 200, 5, 200, 10, 400, 400, 400, 10, 200, 5, 200, 400, 400,
        400, 10, 200, 5, 200, 400, 200, 5, 200, 10, 400, 200, 200, 400
    }; // Adjust durations as needed
    
    for(int i = 0; i < melody.size(); i++) {
        if (melody[i] == "P") {
            // Pause (do nothing for rest duration)
            delay(noteDurations[i]);
        } else {
            int frequency = noteFrequencies[melody[i]]; // Assuming noteFrequencies map is defined elsewhere
            tone(buzzerPin, frequency, noteDurations[i]);
        }
    }
}

void LED::init(){
    pinMode(ledPin, OUTPUT);
    digitalWrite(ledPin, LOW);
    ledcAttachPin(ledPin, 1);
    lv_timer_create(led_timer, fade.interval, NULL);
    ledcSetup(1, 5000, 10);
}

void LED::toggle(){
    digitalWrite(ledPin, !digitalRead(ledPin));
}

void LED::flash(){
    fade.status = 1;
    fade.direction = 1;
    fade.duty = 900;
    ledcWrite(1, fade.duty);

}
void LED::pulsing(){
    if(fade.status){
        if(fade.direction){
            fade.duty += fade.increment;
            if(fade.duty >= fade.max){
                fade.duty = fade.max;
                fade.direction = 0;
            }
        }else{
            fade.duty -= fade.increment;
            if(fade.duty <= fade.min){
                fade.duty = fade.min;
                fade.direction = 1;
            }
        }
        ledcWrite(1, fade.duty);
    }
    
}
void led_timer(lv_timer_t * timer) {
    led.pulsing();
}
void VIBRATE::begin() {
    pinMode(vibratePin, OUTPUT);
    digitalWrite(vibratePin, LOW);
}
void VIBRATE::longBuzz() {
    digitalWrite(vibratePin, HIGH);
    lv_timer_create([](lv_timer_t *t) {
        vibrateStop();
        lv_timer_del(t);
    }, 2000, NULL);
}

void VIBRATE::error() {
    digitalWrite(vibratePin, HIGH);
    lv_timer_create([](lv_timer_t *t) {
        vibrateStop();
        lv_timer_del(t);
    }, 1000, NULL);
        
    lv_timer_create([](lv_timer_t *t) {
        vibrateStart();
        lv_timer_del(t);
    }, 1200, NULL);

    lv_timer_create([](lv_timer_t *t) {
        vibrateStop();
        lv_timer_del(t);
    }, 2200, NULL);
}

void VIBRATE::success() {
    digitalWrite(vibratePin, HIGH);
    lv_timer_create([](lv_timer_t *t) {
        vibrateStop();
        lv_timer_del(t);
    }, 700, NULL);
}


void VIBRATE::tap() {
    digitalWrite(vibratePin, HIGH);
    lv_timer_create([](lv_timer_t *t) {
        vibrateStop();
        lv_timer_del(t);
    }, 500, NULL);
}

void VIBRATE::stop() {
    digitalWrite(vibratePin, LOW);
}
void VIBRATE::toggle() {
     digitalWrite(vibratePin, !digitalRead(vibratePin));
}

void vibrateStop(void) {
    VIBRATE vibe;
    digitalWrite(vibe.vibratePin, LOW);
}
void vibrateStart(void) {
    VIBRATE vibe;
    digitalWrite(vibe.vibratePin, HIGH);
}