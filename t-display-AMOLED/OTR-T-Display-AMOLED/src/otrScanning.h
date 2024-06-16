// otr_scanning.h
#ifndef OTRSCANNING_H
#define OTRSCANNING_H

#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"
//#include "otrFeedback.h"
//#include "otrData.h"
//#include "otrDisplay.h"
// pins
#define RXD1 10
#define TXD1 11

#define SERIAL1_BAUD 9600
#define SERIAL1_TIMEOUT 50


void serial1Initialise(void);


#endif