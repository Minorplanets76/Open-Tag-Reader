#include "otrScanning.h"





void serial1Initialize(void) {
    Serial.begin(115200);
    Serial1.begin(SERIAL1_BAUD, SERIAL_8N1, RXD1, TXD1);
    Serial1.setTimeout(SERIAL1_TIMEOUT);
    printProgressDot();
}

bool validateRFID(const char* RFID)    {
    if (strlen(RFID) != 16) {
        //not a dog by any chance?
        Serial.println("RFID is not 16 characters long");
        Serial.print("Length: ");
        Serial.println(strlen(RFID));
        playScanBad();
        // tft.fillScreen(TFT_RED);
        // tft.setTextColor(TFT_BLACK);
        // tft.drawCentreString("INVALID", LV_SCREEN_HEIGHT/2, 55,4);
        // tft.setTextColor(TFT_WHITE, TFT_BLACK);
        return false;
    }
    if (strncmp(RFID, "940", 3) != 0) {
        Serial.println("RFID does not start with 940");
        String dogName = findDogName(RFID);
        if(dogName == "NOT FOUND") {
            Serial.print("It's not a known dog, but starts with: ");
            Serial.println(strncmp(RFID, "940", 3));
        } else {
            Serial.print("Dog name: ");
            Serial.println(dogName);
        }
        playScanGood();
        // tft.fillScreen(TFT_RED);
        // tft.setTextColor(TFT_BLACK);
        // tft.drawCentreString("INVALID", LV_SCREEN_HEIGHT/2, 55,4);
        // tft.setTextColor(TFT_WHITE, TFT_BLACK);
        return false;
    }
    return true;
}

String findDogName(const char* RFID) {
    File file = LittleFS.open("dogs.csv", FILE_READ);
    String dogName = ""; // Initialize the dog name variable

    if (file) {
        while (file.available()) {
            String line = file.readStringUntil('\n');
            if (line.startsWith(RFID)) {
                // Extract the dog name from the CSV line
                int commaIndex = line.indexOf(',');
                if (commaIndex != -1) {
                    dogName = line.substring(commaIndex + 1);
                    break; // Exit the loop once the RFID match is found
                }
            }
        }
        file.close();
        if (dogName == "") {
            dogName = "NOT FOUND";
        }
    } else {
        Serial.println("Error opening dogs.csv");
    }

    return dogName;
}

int isTagInTagsList(const char* RFID) {
    read_tags_file();
    Serial.println("isInTagsList: ");
    for (int row = 0; row < numTags; row++) {
        if (strcmp(tagRFID[row], RFID) == 0) {
            // Serial.print("Row ");
            // Serial.println(row);
            // Serial.println(tagRFID[row]);
            return row; // Return the row if tag is found
        }
    }

    return -1; // Return -1 if tag is not found in the array
}

bool isTagActive(const char* RFID, int& row) {
    row = isTagInTagsList(RFID);
    if(row != -1)  {

        //Serial.println(tagStatuses[tagStatusVal[row]]);   
        return true;
        
    };
    tft.fillScreen(TFT_ORANGE);
    tft.setTextColor(TFT_BLACK);
    tft.drawCentreString("TAG NOT", LV_SCREEN_HEIGHT/2, 55,4);
    tft.drawCentreString("FOUND", LV_SCREEN_HEIGHT/2, 90,4);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    return false;
}