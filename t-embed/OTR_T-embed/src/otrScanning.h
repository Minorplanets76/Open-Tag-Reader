// otr_scanning.h
#ifndef OTRSCANNING_H
#define OTRSCANNING_H

#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"
#include "otrFeedback.h"
#include "otrData.h"
#include "otrDisplay.h"
// pins
#define RXD1 16
#define TXD1 17

#define SERIAL1_BAUD 9600
#define SERIAL1_TIMEOUT 100

bool validateRFID(const char* inputString);
String findDogName(const char* RFID);
void serial1Initialize(void);


#endif