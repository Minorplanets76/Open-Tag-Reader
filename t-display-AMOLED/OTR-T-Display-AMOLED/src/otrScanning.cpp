#include "otrScanning.h"

bool scanBoard = false; //false for ASCII output (Priority1), true for hex output (WL-134A)



void serial1Initialise(void) {
    //Serial.begin(115200);
    Serial1.begin(SERIAL1_BAUD, SERIAL_8N1, RXD1, TXD1);
    Serial1.setTimeout(SERIAL1_TIMEOUT);
    //printProgressDot();
    Serial.println("Serial1 initialized");
}

void validateRFID(void) {
    
}
