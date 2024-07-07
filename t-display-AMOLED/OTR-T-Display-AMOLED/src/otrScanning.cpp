#include "otrScanning.h"


bool scanBoard = false; //false for ASCII output (Priority1), true for hex output (WL-134A)



void serial1Initialise(void) {
    //Serial.begin(115200);
    Serial1.begin(SERIAL1_BAUD, SERIAL_8N1, RXD1, TXD1);
    Serial1.setTimeout(SERIAL1_TIMEOUT);
    //printProgressDot();
    Serial.println("Serial1 initialised");
}

void validateRFID(void) {
    
}

void WL_134A_RFID:: readPacket()   {
    
    char packet[rfid_packet_size];
    packet[rfid_packet_startcode] = Serial1.read();

    if (packet[rfid_packet_startcode] != 0x02) {
        //not the start
        return;
    }

    uint8_t read;

    read = Serial1.readBytes(&(packet[rfid_packet_ID]), rfid_packet_size - 1);

    if (read != rfid_packet_size - 1) {
        //wrong length
        Serial.println("Wrong length");
        return;
    }
    //calculate checksum
    uint8_t checksum = 0;
    for (int i = rfid_packet_ID; i < rfid_packet_checksum; i++) {
        checksum = checksum ^ packet[i];
    }

    if (checksum != packet[rfid_packet_checksum]) {
        //wrong checksum
        Serial.println("Wrong checksum");
        return;
    }
    if (static_cast<uint8_t>(~checksum) != packet[rfid_packet_checksum_inverted]) {
        //wrong checksum
        Serial.println("Wrong checksum inverted");
        return;
    }
    if (packet[rfid_packet_endcode] != 0x07) {
        // end code missmatch
        if(packet[rfid_packet_endcode] != 0x03) {
        //FDX tag
            Serial.println("FDX tag detected");
            return;
        }  
        Serial.println("Wrong endcode");
        return;
    }


    

    //tagID and countryID are low data first.
    //coverting to decimal requires shifting left by 4 bits
    //start with tagID
    reading.tagID = 0;
    reading.countryID = 0;
    Serial.print("Tag ID: ");
    for (int i =rfid_packet_countryID-1; i >= rfid_packet_ID; --i)
    {
        //each time we get to a new character the current
        //value is shifted left by 4 bits (multiplied by 16)
        reading.tagID = reading.tagID << 4;
        //then we convert the current character to a number and add it to the current value
        reading.tagID += hex_to_uint64(packet[i]);
    }
    Serial.println(reading.tagID);
    //same for countryID
    Serial.print("Country ID: ");
    for (int i = rfid_packet_data-1; i >= rfid_packet_countryID; --i)
    {
        reading.countryID = reading.countryID << 4;
        reading.countryID += hex_to_uint64(packet[i]);
    }
    Serial.println(reading.countryID);
    
    reading.isAnimal = hex_to_uint64(packet[rfid_packet_animal]);

    Serial.print(" Is Animal: ");
    Serial.println(reading.isAnimal);

    snprintf(reading.rfidID, sizeof(reading.rfidID), "%03u %012llu", reading.countryID, reading.tagID);
    Serial.print("RFID ID: ");
    Serial.println(reading.rfidID);

    return;

}

uint64_t WL_134A_RFID:: hex_to_uint64(const char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    return 0;
}

WL_134A_RFID::rfidRead WL_134A_RFID:: getReading() const {
    return reading;
}