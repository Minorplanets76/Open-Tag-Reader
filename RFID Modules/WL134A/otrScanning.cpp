#include "otrScanning.h"



    
    
void RFIDReader::begin(void) {
    
    Serial1.begin(config.baud, SERIAL_8N1, config.RXD1, config.TXD1);
    Serial1.setTimeout(config.timeout);
    
    pinMode(config.RFIDPowerPin, OUTPUT);
    
    activate();
}




void validateRFID(void) {
    
}

void WL_134A_RFID:: readPacket()   {
    
    char packet[rfid_packet_size];
    packet[rfid_packet_startcode] = Serial1.read();
    
    // Packet should be transmitted as follows:
    // LSB                                                                                      MSB
    // ____________________________________________________________________________________________
    // | Start |  Tag | Country | Data | Animal | Reserved | Reserved | Checksum | ~Checksum | End |
    // | Code  |  ID  |   ID    | Flag |  Flag  |          |          |          |           | Code| 
    // | 1byte |10byte|  4byte  | 1byte|  1byte |   4byte  |   6byte  |   1byte  |   1byte   |1byte|
    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // 0       1      11        15     16       17         21         27         28          29    30
    
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
    Serial.print("Is Animal: ");
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

void RFIDReader::activate() {
    digitalWrite(config.RFIDPowerPin, HIGH);
    readerStatus = 1;
    updateScanIcon();
}
void RFIDReader::deactivate() {
    digitalWrite(config.RFIDPowerPin, LOW);
    readerStatus = 0;
    updateScanIcon();
}

void RFIDReader::toggle() {
    readerStatus = !readerStatus;
    
    if(readerStatus) {
        activate();
    } else {
        deactivate();
    }
}


void RFIDReader::updateScanIcon() {
    const lv_img_dsc_t* image;
    extern const lv_img_dsc_t ui_img_contactless_png;
    extern const lv_img_dsc_t ui_img_contactless_off_png;
    Serial.print("Update icon: ");
    Serial.println(readerStatus);
    if(readerStatus) {
        Serial.println("Reader active");
        image = &ui_img_contactless_png;
    } else {
        Serial.println("Reader inactive");
        image = &ui_img_contactless_off_png;
    }
    lv_img_set_src(ui_Main_TopPanelScan,image);
    lv_img_set_src(ui_Scan_TopPanelScan,image);
}