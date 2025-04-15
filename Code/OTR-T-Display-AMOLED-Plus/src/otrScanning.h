// otr_scanning.h
#ifndef OTRSCANNING_H
#define OTRSCANNING_H

#include <Arduino.h>
#include "FS.h"
#include "LittleFS.h"
#include <LV_Helper.h>
#include <lvgl.h>
#include "ui/ui.h"
//#include "otrFeedback.h"
//#include "otrData.h"
//#include "otrDisplay.h"
// pins




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

class RFIDReader {
    public:
        struct RFIDConfig{
            const uint8_t RFIDPowerPin = 42;
            const uint8_t TXD1 = 11;
            const uint8_t RXD1 = 10;
            const uint16_t baud = 9600;
            const uint16_t timeout = 50;
            const uint8_t messageLength = 30;

        };
        String scanError;
        bool readerBoard;
        bool readerStatus = 0;
        void begin(void);
        void activate();
        void deactivate();
        void toggle();
        void updateScanIcon();
    private:
         RFIDConfig config;
         
         
};




#endif