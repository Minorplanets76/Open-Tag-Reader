// otrFeedbaack.h
#ifndef OTRFEEDBACK_H
#define OTRFEEDBACK_H

#include <Arduino.h>

const int vbusPin = 1;
void playClickGoesTheShears(int buzzerPin);
void playWaltzingMatilda(int buzzerPin);
float readVbusVoltage();
uint16_t batteryVoltage();
void updateBatteryImage();

#endif
