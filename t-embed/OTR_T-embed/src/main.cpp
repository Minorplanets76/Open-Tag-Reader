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
#include "otrDisplay.h"
#include "otrData.h"
#include "ui/ui.h"

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




bool isTagActive(const char* inputString, int& row);
void add_tags_from_file(void);


int isTagInTagsList(const char* RFID);
void displaySuccessfulScan(int& row);

void add_scan_to_list(const char* path, const char* filename, const char* record);


void handleRoot(AsyncWebServerRequest *request);
void handleValues(AsyncWebServerRequest *request);










bool scanProcessed = false;





void setup()
{

    serial1Initialize();
    rtc_init();
    initDisplay();
    initFileSystem();
    initAudio();
    initLED();
    initButton();
   

 

// Serial.print("Connecting to WiFi ");  
// WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
// while (WiFi.status() != WL_CONNECTED)   {
//     delay(500);
//     Serial.print(".");
// }
// Serial.println("\nWifi connected.");
// Serial.print("IP address: ");
// Serial.println(WiFi.localIP());




print_local_time();

// delay(1000);
// Serial.printf("LittleFS totalBytes : %d kb\r\n", LittleFS.totalBytes() / 1024);
// Serial.printf("LittleFS usedBytes : %d kb\r\n", LittleFS.usedBytes() / 1024);
// tft.setRotation(2);
// tft.fillScreen(TFT_BLACK);
// SD_init();





// section for web server
// server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
//     handleRoot(request);
// });
// server.on("/values", HTTP_GET, [](AsyncWebServerRequest *request) {
//     handleValues(request);
// });
// server.serveStatic("/html", LittleFS, "/");
// // Route to serve Black.png
// server.on("/Black.png", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send(LittleFS, "/html/BLACK.png", "image/png");
// });
// server.on("/nlis_white.png", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send(LittleFS, "/html/nlis_white.png", "image/png");
// });
// server.on("/textfile", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send(LittleFS, "/html/test2.txt", "text/plain");  // Serve the example.txt file as text/plain content type
// });
// server.begin();
// Serial.println("Server started");


// splash_screen();
// delay(5000);


//read_tag_status_list();
read_tag_group_list();
read_tag_location_list();
read_tags_file();
//add_tags_from_file();
//printFileContents("/test_bucket2.csv");


//createExampleFile();
ui_init();
}

void loop()
{
    audio->loop();
    lv_handler();
    
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
    tft.drawCentreString("OPEN TAG", LV_SCREEN_WIDTH/2, 10, 4);
    tft.drawCentreString("READER",LV_SCREEN_WIDTH/2,35,4);
    
    tft.setCursor(5,70,2);
    // Display the temperature
	//tft.printf(" Temperature: %.1f `C\r\n", rtc.getTemperature());


    float volt = (analogRead(PIN_BAT_VOLT) * 2 * 3.3 ) / 4095;
    tft.printf(" Battery voltage: %.2f V\r\n", volt);

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











