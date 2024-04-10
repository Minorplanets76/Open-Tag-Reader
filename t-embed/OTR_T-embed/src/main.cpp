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


TFT_eSPI tft = TFT_eSPI();
Audio *audio;
RTC_DS3231 rtc;
RotaryEncoder encoder(PIN_ENCODE_A, PIN_ENCODE_B, RotaryEncoder::LatchMode::TWO03);
OneButton button(PIN_ENCODE_BTN, true);
APA102<PIN_APA102_DI, PIN_APA102_CLK> ledStrip;
EventGroupHandle_t global_event_group;
QueueHandle_t led_setting_queue;

void SD_init(void);
void splash_screen(void);
void read_bucket_file(void);
bool read_tags_file(void);
void read_tag_group_list();
void read_tag_location_list(); 
void read_active_tags_file(void);
void read_tag_status_list(void);
void read_bucket_file(int(bucketRows),String(buckRFID),String(buckNLISID), String(buckVisualID), 
                      String(buckIssueDate),String(buckColour), int bucketTags);
void set_RTC(byte& year, byte& month, byte& date, byte& dOW,
                  byte& hour, byte& minute, byte& second);
void printLocalTime(void);
bool validateRFID(const char* inputString);
bool isTagActive(const char* inputString, int& row);
void read_bucket_file(void);
void add_tags_from_file(void);
void copyFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, const char* destinationFilename);
void appendNewLine(const char* filePath, char** bucketRow);
void moveFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, const char* destinationFilename);
void printFileContents(const char* filePath);
String findDogName(const char* RFID);
int isTagInTagsList(const char* RFID);
void displaySuccessfulScan(int& row);
void led_task(void *param);
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v);

#define RXD1 16
#define TXD1 17



bool pmFlag;
byte year;
byte month;
byte date;
byte dOW;
byte hour;
byte minute;
byte second;

char RFID[17];
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
uint16_t LEDmode = 0;
uint16_t newLEDmode = 0;
uint16_t brightness = 30;

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
  Serial.begin(115200);
  Serial1.begin(9600, SERIAL_8N1, RXD1, TXD1);
  Serial1.setTimeout(100);

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

  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS"); delay(1000);
    return;
  }

  audio = new Audio(0, 3, 1);
  audio->setPinout(PIN_IIS_BCLK, PIN_IIS_WCLK, PIN_IIS_DOUT);
  audio->setVolume(21); // 0...21
  if (!audio->connecttoFS(LittleFS, "/sounds/start.mp3")) {
    Serial.println("Start.mp3 not found"); while (1)delay(1000);
  }
 
  Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);  //initialise RTC
  rtc.begin();
  if (!rtc.begin())     {
    Serial.println("Couldn't find RTC");
    return;
  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
while (WiFi.status() != WL_CONNECTED)   {
    delay(500);
    Serial.print(".");
}
Serial.println("\nWifi connected.");

configTime(GMT_OFFSET_SEC, DAY_LIGHT_OFFSET_SEC, NTP_SERVER1, NTP_SERVER2);
struct tm timeinfo;
if (!getLocalTime(&timeinfo))   {
    Serial.println("Failed to obtain time");
    return;
}
rtc.adjust(DateTime(timeinfo.tm_year+1900,timeinfo.tm_mon+1,timeinfo.tm_mday,
                    timeinfo.tm_hour, timeinfo.tm_min,timeinfo.tm_sec));
printLocalTime();

delay(1000);
Serial.printf("LittleFS totalBytes : %d kb\r\n", LittleFS.totalBytes() / 1024);
Serial.printf("LittleFS usedBytes : %d kb\r\n", LittleFS.usedBytes() / 1024);
tft.setRotation(2);
tft.fillScreen(TFT_BLACK);
SD_init();

led_setting_queue = xQueueCreate(5, sizeof(uint16_t));

xTaskCreatePinnedToCore(led_task, "led_task", 1024 * 2, led_setting_queue, 0, NULL, 0);

splash_screen();



read_tag_status_list();
read_tag_group_list();
read_tag_location_list();
read_tags_file();
//add_tags_from_file();
//printFileContents("/test_bucket2.csv");

LEDmode = 4;

}

void loop()
{
    audio->loop();
    if (Serial.available()) {
        String teststr = Serial.readString();  //read until timeout
        teststr.trim();
        audio->connecttoFS(LittleFS, "/sounds/scanbad.mp3");
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
                LEDmode = 3;
                newLEDmode = (LEDmode << 6) | (brightness & 0x3F);
                xQueueSend(led_setting_queue, &newLEDmode, portMAX_DELAY); // Send the new LED mode to the queue
            
        } else{
            int row = 0;
            if(!isTagActive(RFID,row))    {
                Serial.println("Not active");
                audio->connecttoFS(LittleFS, "/sounds/scanbad.mp3");
                Serial.println("Tag not found in active_tags.csv");
                LEDmode = 2;
                newLEDmode = (LEDmode << 6) | (brightness & 0x3F);
                xQueueSend(led_setting_queue, &newLEDmode, portMAX_DELAY); // Send the new LED mode to the queue
            } else {
                Serial.println("Active");
                displaySuccessfulScan(row);
                audio->connecttoFS(LittleFS, "/sounds/scangood.mp3");
                LEDmode = 1;
                newLEDmode = (LEDmode << 6) | (brightness & 0x3F);
                xQueueSend(led_setting_queue, &newLEDmode, portMAX_DELAY); // Send the new LED mode to the queue
            }
        }
    }
}

void splash_screen(void) {  //Screen with memory parameters
    tft.fillScreen(TFT_BLACK);
    tft.drawCentreString("OPEN TAG", LV_SCREEN_HEIGHT/2, 10, 4);
    tft.drawCentreString("READER",LV_SCREEN_HEIGHT/2,35,4);
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    tft.setCursor(5,70,2);
    tft.printf("SD Card Size: %lluMB\n", cardSize);
    uint64_t cardUsed = SD_MMC.usedBytes() / (1024 * 1024);
    tft.printf(" SD Card used: %lluMB\n", cardUsed);
    tft.printf(" psram size: %d kb\r\n", ESP.getPsramSize() / 1024);
    tft.printf(" FLASH size: %d kb\r\n", ESP.getFlashChipSize() / 1024);
    tft.printf(" LittleFS total: %d kb\r\n", LittleFS.totalBytes() / 1024);
    tft.printf(" LittleFS used: %d kb\r\n", LittleFS.usedBytes() / 1024);
    // Display the temperature
	tft.printf(" Temperature: %.1f `C\r\n", rtc.getTemperature());
    //Dsiplay RTC time and date
    DateTime now = rtc.now();
    if (now.isPM()) {
        tft.printf(" RTC Time: %02d:%02d:%02d PM\r\n", now.hour()-12, now.minute(), now.second());
    } else{
        tft.printf(" RTC Time: %02d:%02d:%02d AM\r\n", now.hour(), now.minute(), now.second());
    }
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))   {
        Serial.println("Failed to obtain time");
        return;
    }
    tft.printf(" NTP Time: %02d:%02d:%02d\r\n", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    tft.printf(" RTC Date: %d/%d/%d \r\n", now.day(),now.month(),now.year());

    float volt = (analogRead(PIN_BAT_VOLT) * 2 * 3.3 ) / 4095;
    tft.printf(" Battery voltage: %.2f V\r\n", volt);
    Serial.printf("Current Date: %02d/%02d/%04d\r\n", timeinfo.tm_mday, timeinfo.tm_mon+1, timeinfo.tm_year+1900);
  }

void SD_init(void)
{
    pinMode(PIN_SD_CS, OUTPUT);
    digitalWrite(PIN_SD_CS, 1);
    SD_MMC.setPins(PIN_SD_SCK, PIN_SD_MOSI, PIN_SD_MISO);
    if (!SD_MMC.begin("/sdcard", true)) {
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD_MMC.cardType();

    if (cardType == CARD_NONE) {
        Serial.println("No SD card attached");
        return;
    }
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
        Serial.println("MMC");
    } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    delay(500);
}

void printLocalTime()   {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))   {
        Serial.println("Failed to obtain time");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
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

    Serial.print("No. Statuses: ");
    Serial.println(numStatuses);

    for(int i = 0; i < numStatuses; i++)  {
        Serial.println(tagStatuses[i]);
    }

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

    Serial.print("No. Groups: ");
    Serial.println(numGroups);

    for(int i = 0; i < numGroups; i++)  {
        Serial.println(groups[i]);
    }

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

    Serial.print("No. Locations: ");
    Serial.println(numLocations);

    for(int i = 0; i < numLocations; i++)  {
        Serial.println(locations[i]);
    }

    tagLocationFile.close();
}

bool validateRFID(const char* RFID)    {
    if (strlen(RFID) != 16) {
        //not a dog by any chance?
        Serial.println("RFID is not 16 characters long");
        Serial.print("Length: ");
        Serial.println(strlen(RFID));
        audio->connecttoFS(LittleFS, "/sounds/scanbad.mp3");
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_BLACK);
        tft.drawCentreString("INVALID", LV_SCREEN_HEIGHT/2, 55,4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        return false;
    }
    if (strncmp(RFID, "940", 3) != 0) {
        // it might be a dog
        if(findDogName(RFID) == "NOT FOUND") {
            
        }
        Serial.println("RFID does not start with 940");
        Serial.print("It's not a known dog, but starts with: ");
        Serial.println(strncmp(RFID, "940", 3));
        audio->connecttoFS(LittleFS, "/sounds/scanbad.mp3");
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_BLACK);
        tft.drawCentreString("INVALID", LV_SCREEN_HEIGHT/2, 55,4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        return false;
    }
    return true;
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

void copyFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, const char* destinationFilename) {
    String destinationPath = String(destinationDirectory) + "/" + String(destinationFilename);

    File sourceFile = LittleFS.open(sourcePath, "r");
    if (!sourceFile) {
        Serial.println("Failed to open source file.");
        return;
    }

    if (!SD_MMC.exists(destinationDirectory)) {
        SD_MMC.mkdir(destinationDirectory);
    }

    File destinationFile = SD_MMC.open(destinationPath.c_str(), "w");
    if (!destinationFile) {
        Serial.println("Failed to create destination file.");
        sourceFile.close();
        return;
    }

    while (sourceFile.available()) {
        destinationFile.write(sourceFile.read());
    }

    sourceFile.close();
    destinationFile.close();
}

void appendNewLine(const char* filePath, char** bucketRow) {
    File file = LittleFS.open(filePath, "a");
    if (!file) {
    Serial.println("Failed to open file for appending.");
    return;
    }
    file.print(bucketRow[0]);
    file.print(",");
    file.print(bucketRow[1]);
    file.print(",");
    file.print(bucketRow[2]);
    file.print(",");
    file.print(bucketRow[3]);
    file.print(",");
    file.print(bucketRow[4]);
    file.print(",\"0\",\"0\",\"\",\"0\",\"0\""); // add placeholders for gender, status, name, group and location
    file.println(); // Write the line with a newline character
    file.close();
}

void moveFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, const char* destinationFilename) {
    String destinationPath = String(destinationDirectory) + "/" + String(destinationFilename);

    File sourceFile = LittleFS.open(sourcePath, "r");
    if (!sourceFile) {
        Serial.println("Failed to open source file.");
        return;
    }

    if (!SD_MMC.exists(destinationDirectory)) {
        SD_MMC.mkdir(destinationDirectory);
    }

    File destinationFile = SD_MMC.open(destinationPath.c_str(), "w");
    if (!destinationFile) {
        Serial.println("Failed to create destination file.");
        sourceFile.close();
        return;
    }

    while (sourceFile.available()) {
        destinationFile.write(sourceFile.read());
    }

    sourceFile.close();
    destinationFile.close();

    // Remove the source file after moving
    LittleFS.remove(sourcePath);
}
void printFileContents(const char* filePath) {
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        Serial.println("Failed to open file.");
        return;
    }

    int lineNumber = 1;
    Serial.print(String(lineNumber) + ": ");
    while (file.available()) {
        char c = file.read();
        Serial.print(c);
        if (c == '\n') {
            lineNumber++;
            Serial.print(String(lineNumber) + ": ");
        }
    }

    file.close();
}

int isTagInTagsList(const char* RFID) {
    read_tags_file();
    Serial.println("isInTagsList: ");
    for (int row = 0; row < numTags; row++) {
        if (strcmp(tagRFID[row], RFID) == 0) {
            Serial.print("Row ");
            Serial.println(row);
            Serial.println(tagRFID[row]);
            return row; // Return the row if tag is found
        }
    }

    return -1; // Return -1 if tag is not found in the array
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


void led_task(void *param) {
    const uint8_t ledSort[7] = {2, 1, 0, 6, 5, 4, 3};
    const uint16_t ledCount = 7;
    rgb_color colors[ledCount];
    uint8_t brightness = 30;
    uint16_t temp;
    int8_t last_led = 0;
    EventBits_t bit;

    while (1) {
        if (xQueueReceive(led_setting_queue, &temp, 0)) {
            LEDmode = (temp >> 6) & 0xF;
            brightness = temp & 0x3F;

            Serial.printf("temp : 0x%X\r\n", temp);
            Serial.printf("LEDmode : 0x%X\r\n", LEDmode);
            Serial.printf("brightness : 0x%X\r\n", brightness);
        }

        switch (LEDmode) {
            case 0:
                // LEDs off
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[i] = rgb_color(0, 0, 0);
                }
                ledStrip.write(colors, ledCount, brightness);
                break;
            case 1:
                // LEDs solid green
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[i] = rgb_color(0, 255, 0); // Green color
                }
                ledStrip.write(colors, ledCount, brightness);
                break;
            case 2:
                // LEDs solid orange
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[i] = rgb_color(255, 165, 0); // Orange color
                }
                ledStrip.write(colors, ledCount, brightness);
                break;
            case 3:
                // LEDs solid red
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[i] = rgb_color(255, 0, 0); // Red color
                }
                ledStrip.write(colors, ledCount, brightness);
                break;
            case 4:
                // LEDs green sequentially
                for (uint16_t i = 0; i < ledCount; i++) {
                    colors[ledSort[i]] = rgb_color(0, 255, 0); // set current led green
                    ledStrip.write(colors, ledCount, brightness);
                    colors[ledSort[i]] = rgb_color(0, 0, 0);
                    delay(100);
                }
                break;
            default:
                break;
        }

        delay(10);
    }
}


rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch ((h / 60) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    }
    return rgb_color(r, g, b);
}



