// otrFeedbaack.h
#ifndef OTRFEEDBACK_H
#define OTRFEEDBACK_H

#include <Arduino.h>
#include <lvgl.h>

const int vbusPin = 1;
void playClickGoesTheShears(int buzzerPin);
void playWaltzingMatilda(int buzzerPin);
float readVbusVoltage();
uint16_t batteryVoltage();
void updateBatteryImage();
void led_timer(lv_timer_t * timer);
void vibrateStop(void);
void vibrateStart(void);

class LED {
    public:
        int ledPin = 39;
        void init();
        void toggle();
        void flash();
        void pulsing();       
                struct dutyCycle{
            bool status = false; //on/off
            bool direction = 0; //0 down, 1 up
            uint16_t duty = 1024;
            const uint8_t increment = 1;
            const uint16_t min = 100;
            const uint16_t max = 1023;
            const uint16_t interval = 50;
        };
        dutyCycle fade;
        
    private:

};

class VIBRATE {
    public:
        const int vibratePin = 40;
        static void timer(lv_timer_t * t);
        void begin();
        void longBuzz();
        void tap();
        void error();
        void success();
        void toggle();
        void stop();
        

};

#endif
