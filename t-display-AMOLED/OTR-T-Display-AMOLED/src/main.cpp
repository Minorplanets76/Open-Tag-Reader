/**
 * Minimal Test case C.Chad 2024
 */
#include <LilyGo_AMOLED.h>
#include <LV_Helper.h>
#include <lvgl.h>
#include "ui/ui.h"
#include "otrFileHandling.h"
#include "otrScanning.h"
#include "otrFeedback.h"
#include "otrTime.h"

LilyGo_Class amoled;
lv_obj_t *label1;
lv_obj_t *testButtonLabel;
lv_obj_t *labelSlider;
lv_obj_t *touchTest;
uint8_t btnPin = 0;
int ledPin = 39;
void toggleLed(int ledPin);
int buzzerPin = 16;
void playNote(int buzzerPin);

unsigned int naturalNoteFrequencies[] = {131, 147, 165, 175, 196, 220, 247, 262, 294, 330, 349, 392, 440, 494, 
                                        523, 587, 659, 698, 784, 880, 988, 1047, 1175, 1319, 1397, 1568, 1760, 1976};
int noteIndex = 0;
uint8_t rotation = 1;
const char *format_string = "#0000ff X:%d#\n #990000 Y:%d#\n #3d3d3d Size:%s# ";
char RFID[17];
int vibratePin = 40;
int RFIDBuzzerPin = 41;
int RFIDPowerPin = 42;



void lv_example_get_started_1(void)
{
    

    /*Touch Test*/
    touchTest = lv_label_create(lv_scr_act());
    lv_obj_align(touchTest, LV_ALIGN_BOTTOM_MID, 0, -50);
    lv_obj_set_style_text_font(touchTest, &lv_font_montserrat_28, 0);
    lv_label_set_text(touchTest, "Touch Test");

    amoled.setHomeButtonCallback([](void *ptr) {
        Serial.println("Home key pressed!");
        static uint32_t checkMs = 0;
        if (millis() > checkMs) {
            uint16_t battVoltageTest = amoled.getBattVoltage();
            double battVoltageInVolts = battVoltageTest / 1000.0;
            uint16_t vbusVoltageMV = readVbusVoltage();
            double vbusVoltage = vbusVoltageMV / 1000.0;
            lv_label_set_text_fmt(touchTest, "Battery: %.1fV\nVBUS: %.1fV", battVoltageInVolts, vbusVoltage);
            //playWaltzingMatilda(buzzerPin);
            
        }
        checkMs = millis() + 200;
        lv_timer_create([](lv_timer_t *t) {
            lv_label_set_text(touchTest, "Touch Test");
            
            lv_timer_del(t);
        }, 2000, NULL);
    }, NULL);

}


void toggleLed(int ledPin)
{
    digitalWrite(ledPin, !digitalRead(ledPin));
    digitalWrite(RFIDPowerPin, !digitalRead(RFIDPowerPin));
}

void playNote(int buzzerPin)
{
    unsigned int noteFrequency = naturalNoteFrequencies[noteIndex];
    tone(buzzerPin, noteFrequency, 100);
    noteIndex = (noteIndex + 1) % (sizeof(naturalNoteFrequencies) / sizeof(naturalNoteFrequencies[0]));
}



void setup()
{
    Serial.begin(115200);

    bool rslt = false;
    // Automatically determine the access device
    rslt = amoled.begin();

    if (!rslt) {
        while (1) {
            Serial.println("The board model cannot be detected, please raise the Core Debug Level to an error");
            delay(1000);
        }
    }

    beginLvglHelper(amoled);
    amoled.setRotation(rotation);
    lv_disp_drv_t *drv = lv_disp_get_default()->driver;
    drv->hor_res = amoled.width();
    drv->ver_res = amoled.height();
    lv_disp_drv_update(lv_disp_get_default(), drv);
    
    ui_init();

    lv_example_get_started_1(); 

    pinMode(ledPin, OUTPUT);
    pinMode(buzzerPin, OUTPUT);
    pinMode(RFIDPowerPin, OUTPUT);
    digitalWrite(RFIDPowerPin, HIGH);
    pinMode(vbusPin, INPUT);
    pinMode(vibratePin, OUTPUT);
    digitalWrite(vibratePin, LOW);

    pinMode(RFIDBuzzerPin, INPUT);

    digitalWrite(ledPin, LOW);
    digitalWrite(buzzerPin, LOW);
    //playClickGoesTheShears(buzzerPin);
    SD_init();
    serial1Initialise();
    digitalWrite(vibratePin, HIGH);
    delay(500);
    digitalWrite(vibratePin, LOW);
}


void loop()
{

    lv_task_handler();
    // static unsigned long previousMillis = 0;
    // const long interval = 500;
    // unsigned long currentMillis = millis();

    // if (currentMillis - previousMillis >= interval) {
    //     previousMillis = currentMillis;
    //     digitalWrite(ledPin, !digitalRead(ledPin));
    // }
    lv_indev_t *indev = lv_indev_get_next(NULL);
    lv_point_t  point;

    if (Serial1.available()) {
        String readRFID = Serial1.readString(); //read until timeout
        readRFID.toCharArray(RFID, 17);
        tone(buzzerPin, 2000, 100);
        Serial.println(RFID);
        lv_obj_set_style_text_font(ui_Main_Label1, &lv_font_montserrat_22, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_label_set_text_fmt(ui_Main_Label1, "SCAN OK \n %s",RFID);
    }
}
