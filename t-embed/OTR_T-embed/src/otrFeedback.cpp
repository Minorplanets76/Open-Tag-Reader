#include "otrFeedback.h"



extern Audio *audio;

extern QueueHandle_t led_setting_queue;
extern APA102<PIN_APA102_DI, PIN_APA102_CLK> ledStrip;
extern OneButton button;
extern RotaryEncoder encoder;
void initAudio() {
    audio = new Audio(0, 3, 1);
    audio->setPinout(PIN_IIS_BCLK, PIN_IIS_WCLK, PIN_IIS_DOUT);
    audio->setVolume(21); // 0...21
    if (!audio->connecttoFS(LittleFS, "/sounds/start.mp3")) {
        Serial.println("Start.mp3 not found"); while (1)delay(1000);
    }
}

void playScanGood() {
    audio->connecttoFS(LittleFS, "/sounds/scangood.mp3");
}

void playScanBad() {
    audio->connecttoFS(LittleFS, "/sounds/scanbad.mp3");
}

void initLED(void) {
    
    led_setting_queue = xQueueCreate(5, sizeof(uint16_t));
    xTaskCreatePinnedToCore(led_task, "led_task", 1024 * 2, led_setting_queue, 0, NULL, 0);
}

void led_task(void *param) {
    const uint8_t ledSort[7] = {2, 1, 0, 6, 5, 4, 3};
    const uint16_t ledCount = 7;
    rgb_color colors[ledCount];
    uint8_t brightness = 30;
    uint16_t temp;
    int8_t last_led = 0;
    EventBits_t bit;

    while (1) {
        if (xQueueReceive(led_setting_queue, &temp, 0)) {
            LEDmode = (temp >> 6) & 0xF;
            brightness = temp & 0x3F;
        }
        switch (LEDmode) {
            case 0:
                // LEDs off
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[i] = rgb_color(0, 0, 0);
                }
                ledStrip.write(colors, ledCount, brightness);
                break;
            case 1:
                // LEDs solid green
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[i] = rgb_color(0, 255, 0); // Green color
                }
                ledStrip.write(colors, ledCount, brightness);
                break;
            case 2:
                // LEDs solid orange
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[i] = rgb_color(255, 165, 0); // Orange color
                }
                ledStrip.write(colors, ledCount, brightness);
                break;
            case 3:
                // LEDs solid red
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[i] = rgb_color(255, 0, 0); // Red color
                }
                ledStrip.write(colors, ledCount, brightness);
                break;
            case 4:
                // LEDs green sequentially
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[ledSort[i]] = rgb_color(0, 255, 0); // set current led green
                    ledStrip.write(colors, ledCount, brightness);
                    colors[ledSort[i]] = rgb_color(0, 0, 0);
                    delay(100);
                }
                break;
            default:
                break;
        }

        delay(10);
    }
}

rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch ((h / 60) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    }
    return rgb_color(r, g, b);
}

void changeLEDmode(uint8_t LEDmode) {
    uint8_t brightness = 30;
    newLEDmode = (LEDmode << 6) | (brightness & 0x3F);
    xQueueSend(led_setting_queue, &newLEDmode, portMAX_DELAY); // Send the new LED mode to the queue
}

void initButton(void) {
    
    button.attachClick(button_pressed);
}

void button_pressed() { 
    Serial.println("Button pressed - Enter Scan Mode!");
    changeLEDmode(4); // LEDs green sequentially
    digitalWrite(PIN_VIBRATE, HIGH);
}

void getBatteryVoltage() {
    float volts = analogRead(PIN_BAT_VOLT) *2 * 3.3 / 4095;
    Serial.printf(" Battery voltage: %.2f V\r\n", volts);
}

void getBatteryChargeState() {
    // Above 4.2V is charging
    //
    // 4.2 volts 100%
    // 4.1 about 90%
    // 4.0 about 80%
    // 3.9 about 60%
    // 3.8 about 40%
    // 3.7 about 20%
    // 3.6 empty for practical purposes.
    float volts = analogRead(PIN_BAT_VOLT) *2 * 3.3 / 4095;
    if (volts > 4.4) {
        Serial.printf(" Battery charge state: Charging\r\n");
    } else if (volts > 4.2) {
        Serial.printf(" Battery charge state: 100%%\r\n");
    } else if (volts > 4.1) {
        Serial.printf(" Battery charge state: 90%%\r\n");
    } else if (volts > 4.0) {
        Serial.printf(" Battery charge state: 80%%\r\n");
    } else if (volts > 3.9) {
        Serial.printf(" Battery charge state: 60%%\r\n");
    } else if (volts > 3.8) {
        Serial.printf(" Battery charge state: 40%%\r\n");
    } else if (volts > 3.7) {
        Serial.printf(" Battery charge state: 20%%\r\n");
    } else {
        Serial.printf(" Battery charge state: Empty\r\n");
    }
}