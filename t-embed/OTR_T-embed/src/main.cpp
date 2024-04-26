#include <Arduino.h>
#include "TFT_eSPI.h" // https://github.com/Bodmer/TFT_eSPI
#include "pin_config.h"
#include "Audio.h"
#include "FS.h"
#include "LittleFS.h"
#include "SD_MMC.h"
#include "RTClib.h"
#include "Wire.h"
#include <WiFi.h>
#include "time.h"
#include <CSV_Parser.h>
#include <LITTLEFS.h>
#include "APA102.h" 
#include <OneButton.h> 
#include <RotaryEncoder.h>
#include "ESPAsyncWebServer.h"
#include <AsyncTCP.h>
#include <ArduinoJson.h>
#include "otrTime.h"
#include "credentials.h"
#include "otrFileHandling.h"
#include "otrFeedback.h"
#include "otrScanning.h"

TFT_eSPI tft = TFT_eSPI();

RTC_DS3231 rtc;
Audio *audio;
RotaryEncoder encoder(PIN_ENCODE_A, PIN_ENCODE_B, RotaryEncoder::LatchMode::TWO03);
OneButton button(PIN_ENCODE_BTN, true);
APA102<PIN_APA102_DI, PIN_APA102_CLK> ledStrip;
EventGroupHandle_t global_event_group;
QueueHandle_t led_setting_queue;
AsyncWebServer server(80);



void splash_screen(void);
void read_bucket_file(void);
bool read_tags_file(void);
void read_tag_group_list();
void read_tag_location_list(); 
void read_active_tags_file(void);
void read_tag_status_list(void);
void read_bucket_file(int(bucketRows),String(buckRFID),String(buckNLISID), String(buckVisualID), 
                      String(buckIssueDate),String(buckColour), int bucketTags);



bool isTagActive(const char* inputString, int& row);
void read_bucket_file(void);
void add_tags_from_file(void);


int isTagInTagsList(const char* RFID);
void displaySuccessfulScan(int& row);

void add_scan_to_list(const char* path, const char* filename, const char* record);


void handleRoot(AsyncWebServerRequest *request);
void handleValues(AsyncWebServerRequest *request);






bool pmFlag;
int year;
int month;
int date;
int hour;
int minute;
int second;

char RFID[17] = "No Data";
String NLISID = "No Data";
String VisualID = "No Data";
String currentStatus = "No Data";
String NAME = "No Data";
String GROUP = "No Data";
String LOCATION = "No Data";
String COLOUR = "No Data";
char **tagRFID;
char **tempRFID;
char **tagVisual_ID;
char **tagNLISID;
char **tagColour;
char **tagAppliedDate;
int numStatuses = 0;
int numGroups = 0;
int numLocations = 0;
String tagStatuses[10];
String groups[20];
String locations[10];
String gender[3] = {"None", "Male", "Female"};
char **tagName;
uint8_t *tagStatusVal;
uint8_t *tagGender;
uint8_t *tagGroup;
uint8_t *tagLocation;
uint16_t numTags;
uint16_t numActiveTags;

bool scanProcessed = false;

typedef struct {
  uint8_t cmd;
  uint8_t data[14];
  uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
  {0x11, {0}, 0 | 0x80},
  {0x3A, {0X05}, 1},
  {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
  {0xB7, {0X75}, 1},
  {0xBB, {0X28}, 1},
  {0xC0, {0X2C}, 1},
  {0xC2, {0X01}, 1},
  {0xC3, {0X1F}, 1},
  {0xC6, {0X13}, 1},
  {0xD0, {0XA7}, 1},
  {0xD0, {0XA4, 0XA1}, 2},
  {0xD6, {0XA1}, 1},
  {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
  {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};




void setup()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    serial1Initialize();
    rtc_init();

    tft.begin();
  // Update Embed initialization parameters
  for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
    tft.writecommand(lcd_st7789v[i].cmd);
    for (int j = 0; j < lcd_st7789v[i].len & 0x7f; j++) {
      tft.writedata(lcd_st7789v[i].data[j]);
    }
    if (lcd_st7789v[i].len & 0x80) {
      delay(120);
    }
  }

  tft.setRotation(3);
  tft.fillScreen(TFT_BLACK);
  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);
  tft.setSwapBytes(true);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("Open Tag Reader", 60, 75, 4);

  initFileSystem();
  initAudio();
  initLED();
  initButton();
 

Serial.print("Connecting to WiFi ");  
WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED)   {
    delay(500);
    Serial.print(".");
}
Serial.println("\nWifi connected.");
Serial.print("IP address: ");
Serial.println(WiFi.localIP());




print_local_time();

delay(1000);
Serial.printf("LittleFS totalBytes : %d kb\r\n", LittleFS.totalBytes() / 1024);
Serial.printf("LittleFS usedBytes : %d kb\r\n", LittleFS.usedBytes() / 1024);
tft.setRotation(2);
tft.fillScreen(TFT_BLACK);
SD_init();





// section for web server
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleRoot(request);
});
server.on("/values", HTTP_GET, [](AsyncWebServerRequest *request) {
    handleValues(request);
});
server.serveStatic("/html", LittleFS, "/");
// Route to serve Black.png
server.on("/Black.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/html/BLACK.png", "image/png");
});
server.on("/nlis_white.png", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/html/nlis_white.png", "image/png");
});
server.on("/textfile", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/html/test2.txt", "text/plain");  // Serve the example.txt file as text/plain content type
});
server.begin();
Serial.println("Server started");


splash_screen();



read_tag_status_list();
read_tag_group_list();
read_tag_location_list();
read_tags_file();
//add_tags_from_file();
//printFileContents("/test_bucket2.csv");


//createExampleFile();

}

void loop()
{
    audio->loop();
    button.tick();
    
    if (Serial.available()) {
        String teststr = Serial.readString();  //read until timeout
        teststr.trim();
        playScanBad();
        tft.fillScreen(TFT_BLACK);
        tft.drawString(teststr, 10, 10, 4);
        Serial.println(teststr);
        // read_bucket_file();

    }     //wait for data available

    if (Serial1.available())  {
        String readRFID;
        readRFID = Serial1.readString(); //read until timeout
        readRFID.replace('_', ' ');
        readRFID.toCharArray(RFID, 17);
        Serial.println(RFID);
        bool isActiveTag = false;
        if(!validateRFID(RFID))   {
            Serial.println("Failed validation check");
            changeLEDmode(3); // LEDs solid red
        } else{
            int row = 0;
            if(!isTagActive(RFID,row))    {
                Serial.println("Not active");
                playScanBad();
                Serial.println("Tag not found in active_tags.csv");
                changeLEDmode(2); // LEDs solid orange
            } else {
                Serial.println("Active");
                displaySuccessfulScan(row);
                NLISID = String(tagNLISID[row]);
                VisualID = String(tagVisual_ID[row]);
                currentStatus = String(tagStatuses[tagStatusVal[row]]);
                Serial.println(currentStatus);
                NAME = String(tagName[row]);
                GROUP = String(groups[tagGroup[row]]);
                LOCATION = String(locations[tagLocation[row]]);
                COLOUR = String(tagColour[row]);
                playScanGood();
                changeLEDmode(1); // LEDs solid green
            }
        }
    }
}

void splash_screen(void) {  //Screen with memory parameters
    tft.fillScreen(TFT_BLACK);
    tft.drawCentreString("OPEN TAG", LV_SCREEN_HEIGHT/2, 10, 4);
    tft.drawCentreString("READER",LV_SCREEN_HEIGHT/2,35,4);
    
    tft.setCursor(5,70,2);
    // Display the temperature
	//tft.printf(" Temperature: %.1f `C\r\n", rtc.getTemperature());


    float volt = (analogRead(PIN_BAT_VOLT) * 2 * 3.3 ) / 4095;
    tft.printf(" Battery voltage: %.2f V\r\n", volt);

  }





void read_bucket_file(int row, char** bucketRow, int& bucketTags) {
    // Reads in a bucket file (Shearwell)
    // Passed a row number and returns values for that row + total rows
    File bucketFile = LittleFS.open("/test_bucket.csv");
    if (!bucketFile) {
        Serial.println("Failed to open 'test_bucket.csv' for reading");
        return;
    }
    
    String csvBucket;
    while (bucketFile.available()) {
        csvBucket += (char)bucketFile.read();
    }
    bucketFile.close();
    
    CSV_Parser bucket(csvBucket.c_str(), "-ssss-s");
    bucket.parseLeftover();
    
    char **bucketRFID = (char**)bucket["RFID"];
    char **bucketNLISID = (char**)bucket["NLISID"];
    char **bucketVisual_ID = (char**)bucket["Visual_ID"];
    char **bucketIssueDate = (char**)bucket["IssueDate"];
    char **bucketTagColour = (char**)bucket["Colour"];
    
    bucketTags = bucket.getRowsCount();
    
    sprintf(bucketRow[0], "\"%s\"", bucketRFID[row]);
    sprintf(bucketRow[1], "\"%s\"", bucketNLISID[row]);
    sprintf(bucketRow[2], "\"%s\"", bucketVisual_ID[row]);
    sprintf(bucketRow[3], "\"%s\"", bucketIssueDate[row]);
    sprintf(bucketRow[4], "\"%s\"", bucketTagColour[row]);
}

bool read_tags_file(void) {
    // file contains all tags
    File tagFile = LittleFS.open("/test_bucket2.csv");
    if (!tagFile) {
        Serial.println("Failed to open 'tags.csv' for reading");
        return false;
    } else {
        //Serial.println("tags.csv opened");
    }

    String csvTags;
    while (tagFile.available()) {
        csvTags += (char)tagFile.read();
    }
    tagFile.close();

    CSV_Parser allTagsList(csvTags.c_str(), "sssssucucsucuc");
    //allTagsList.print();
    
    tagRFID = (char**)allTagsList["RFID"];
    tagVisual_ID = (char**)allTagsList["Visual_ID"];
    tagNLISID = (char**)allTagsList["NLISID"];
    tagColour = (char**)allTagsList["Colour"];
    tagGender = (uint8_t*)allTagsList["Gender"];
    tagStatusVal = (uint8_t*)allTagsList["Status"];
    tagName = (char**)allTagsList["Name"];
    tagGroup = (uint8_t*)allTagsList["Group"];
    tagLocation = (uint8_t*)allTagsList["Location"];
    numTags = allTagsList.getRowsCount();
    
    // Serial.print("From read_tags_file ");
    // Serial.print(numTags);
    // Serial.println(" rows");

    return true;
}

void add_tags_from_file()   {
    //Take bucket file and add any new tags
    Serial.print("Add tags function: ");
    int row = 0;
    int addedCount = 0;
    char* bucketRow[5];
    int bucketTags;
    //1. read bucket file
    read_bucket_file(row, bucketRow, bucketTags);
    Serial.println("Read bucket file complete: ");
    Serial.print("Rows in bucket file: ");
    Serial.println(bucketTags);
    Serial.printf("First row of bucket file: %s,%s,%s,%s,%s\r\n",bucketRow[0], bucketRow[1], 
                    bucketRow[2], bucketRow[3], bucketRow[4]);
    //2. compare rfid with tags list and see if it exists
    char* RFID = strdup(bucketRow[0] + 1);
    RFID[strlen(RFID) - 1] = '\0';
    static bool first_run = true;
    if(read_tags_file())    {
        Serial.print(RFID);
        Serial.println(" already exists");
    }   else    {
        //3. if it doesn't exist, append the tag with unused status
        if(first_run)  {
            first_run = false;
            Serial.println("Making backup ...");
            copyFileFromLittleFStoSD("/test_bucket2.csv", "/backup", "test_bucket2.csv");
            Serial.println("File copied successfully.");
        }
        Serial.println("New tag - adding to tags.csv");
        appendNewLine("/test_bucket2.csv", bucketRow);
        addedCount = 1;
    }
    for(row = 1; row < bucketTags; row++)  {
        read_bucket_file(row, bucketRow, bucketTags);
        if(read_tags_file())    { //change this
            Serial.println("tag already exists");

        }   else    {
            Serial.print(RFID);
            Serial.println("added");
            appendNewLine("/test_bucket2.csv", bucketRow);
            addedCount++;
        }
    }
    Serial.print(addedCount);
    Serial.println(" records added to file");
    moveFileFromLittleFStoSD("/test_bucket.csv", "/Uploaded", "test_bucket.csv");
    Serial.println("File moved successfully.");
}
void read_active_tags_file()   {

}

void read_tag_status_list()   {
    // Placeholder for function to enable user to have list of tag status
    // Currently reads tag_status.csv and populates tagStatus as array
    File tagStatusFile = LittleFS.open("/tag_status.csv");

    if(!tagStatusFile){
        Serial.println("Failed to open tags_status.csv for reading");
        return;
    } else {
        Serial.println("tag_status.csv opened");
    }
    while(tagStatusFile.available())  {
        tagStatuses[numStatuses] = tagStatusFile.readStringUntil('\n');
        numStatuses++;
    }

    // Serial.print("No. Statuses: ");
    // Serial.println(numStatuses);

    // for(int i = 0; i < numStatuses; i++)  {
    //     Serial.println(tagStatuses[i]);
    // }

    tagStatusFile.close();
    
}

void read_tag_group_list()   {
    // Placeholder for function to enable user to have list of tag Groups
    // Currently reads tag_status.csv and populates groups as array
    File tagGroupFile = LittleFS.open("/tag_group.csv");

    if(!tagGroupFile){
        Serial.println("Failed to open tags_group.csv for reading");
        return;
    } else {
        Serial.println("tag_group.csv opened");
    }

    while(tagGroupFile.available())  {
        groups[numGroups] = tagGroupFile.readStringUntil('\n');
        numGroups++;
    }

    // Serial.print("No. Groups: ");
    // Serial.println(numGroups);

    // for(int i = 0; i < numGroups; i++)  {
    //     Serial.println(groups[i]);
    // }

    tagGroupFile.close();
}

void read_tag_location_list()   {
    // Placeholder for function to enable user to have list of tag locations
    // Currently reads tag_location.csv and populates locations as array
    File tagLocationFile = LittleFS.open("/tag_location.csv");

    if(!tagLocationFile){
        Serial.println("Failed to open tags_status.csv for reading");
        return;
    } else {
        Serial.println("tag_status.csv opened");
    }

    while(tagLocationFile.available())  {
        locations[numLocations] = tagLocationFile.readStringUntil('\n');
        numLocations++;
    }

    // Serial.print("No. Locations: ");
    // Serial.println(numLocations);

    // for(int i = 0; i < numLocations; i++)  {
    //     Serial.println(locations[i]);
    // }

    tagLocationFile.close();
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

void displaySuccessfulScan(int& row) {
        tft.fillScreen(TFT_GREEN);
        delay(100);
        tft.fillScreen(TFT_BLACK);
        Serial.println(numTags);
        Serial.println(row);
        tft.drawCentreString(RFID, 85, 10, 2);
        tft.drawCentreString(tagNLISID[row], LV_SCREEN_HEIGHT/2, 30, 2);
        tft.drawCentreString(String(tagVisual_ID[row]).substring(0,4), LV_SCREEN_HEIGHT/2, 100, 4);
        tft.drawCentreString(String(tagVisual_ID[row]).substring(5,8), LV_SCREEN_HEIGHT/2, 140, 8);
        tft.drawCentreString(tagColour[row], LV_SCREEN_HEIGHT/2, 240, 4);
        tft.setTextColor(TFT_YELLOW, TFT_BLACK);
        tft.drawCentreString(tagName[row], LV_SCREEN_HEIGHT/2, 280, 4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        Serial.println(tagVisual_ID[row]);
        Serial.println(tagNLISID[row]);   
}




void add_scan_to_list(const char* path, const char* filename, const char* record) {
    String filepath = String(path) + "/" + String(filename);
    File listFile = LittleFS.open(filepath.c_str(), "a"); // Open the file in append mode

    if (!listFile) {
        Serial.println("Failed to open " + filepath + " for writing");
        return;
    }

    listFile.println(record); // Append the new record to the file
    listFile.close(); // Close the file
}









void handleRoot(AsyncWebServerRequest *request) {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    request->send(404);
    return;
  }

  size_t size = file.size();
  char* buf = new char[size + 1];
  file.readBytes(buf, size);
  file.close();
  buf[size] = '\0'; // Null-terminate the buffer

  // Replace placeholders with actual values
  String htmlContent = String(buf);


  request->send(200, "text/html", htmlContent);
  delete[] buf;
}

void handleValues(AsyncWebServerRequest *request) {
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["RFID"] = RFID;
  jsonDoc["NLISID"] = NLISID;
  jsonDoc["VisualID"] = VisualID;
  jsonDoc["Status"] = currentStatus;
  jsonDoc["Name"] = NAME;
  jsonDoc["Group"] = GROUP;
  jsonDoc["Location"] = LOCATION;
  jsonDoc["Colour"] = COLOUR;

  String response;
  serializeJson(jsonDoc, response);

  request->send(200, "application/json", response);
}











