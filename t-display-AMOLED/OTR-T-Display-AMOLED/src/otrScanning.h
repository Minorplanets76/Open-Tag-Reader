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
const uint8_t rfidMessageLength = 30; //30 bytes

class WL_134A_RFID {
public:
    
    struct rfidRead {
        uint64_t tagID;
        uint16_t countryID;
        bool isAnimal;
        char rfidID[17];
    };
    void readPacket();
    rfidRead getReading() const;

private:
    enum rfidPacket {
        rfid_packet_startcode,
        rfid_packet_ID = 1,
        rfid_packet_countryID = 11,
        rfid_packet_data = 15,
        rfid_packet_animal = 16,
        rfid_packet_reserved1 = 17,
        rfid_packet_reserved2 = 21,
        rfid_packet_checksum = 27,
        rfid_packet_checksum_inverted = 28,
        rfid_packet_endcode = 29,
        rfid_packet_size
    };

    

    uint64_t hex_to_uint64(const char c);

    rfidRead reading;
};




#endif