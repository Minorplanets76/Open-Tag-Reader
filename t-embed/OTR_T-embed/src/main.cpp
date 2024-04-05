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

TFT_eSPI tft = TFT_eSPI();
Audio *audio;
RTC_DS3231 rtc;





void SD_init(void);
void splash_screen(void);
void read_bucket_file(void);
void read_tags_file(void);
void update_tag_status_list(void);
void set_RTC(byte& year, byte& month, byte& date, byte& dOW,
                  byte& hour, byte& minute, byte& second);
void printLocalTime(void);
bool validateRFID(const String& inputString);

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

char **RFID;
char **Visual_ID;
char **NLISID;
char **tagColour;
char **tagStatus;


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


splash_screen();
read_bucket_file();
read_tags_file();
update_tag_status_list();

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
    String otrstr = Serial1.readString();  //read until timeout
    otrstr.trim();
    otrstr.replace("_"," ");
    if(validateRFID(otrstr))    {
        audio->connecttoFS(LittleFS, "/sounds/scangood.mp3");
        audio->loop();
        tft.fillScreen(TFT_GREEN);
        delay(100);
        tft.fillScreen(TFT_BLACK);
        tft.drawCentreString(otrstr, 85, 10, 2);
        Serial.println(otrstr);
        for(int row =0; row < 150; row++)   {
            
            if (String(RFID[row]) == otrstr)    {
                tft.drawCentreString(NLISID[row], LV_SCREEN_HEIGHT/2, 30, 2);
                tft.drawCentreString(String(Visual_ID[row]).substring(0,4), LV_SCREEN_HEIGHT/2, 100, 4);
                tft.drawCentreString(String(Visual_ID[row]).substring(5,8), LV_SCREEN_HEIGHT/2, 140, 8);
                tft.drawCentreString(tagColour[row], LV_SCREEN_HEIGHT/2, 240, 4);
                Serial.print("Tag number: ");
                Serial.println(row+1);
                Serial.println(Visual_ID[row]);
                Serial.println(NLISID[row]);
                break;
            } else {
                
            }
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

void read_bucket_file() {   //Reads in a bucket file (Shearwell)
    File bucketFile = LittleFS.open("/test_bucket.csv");
    if(!bucketFile){
        Serial.println("Failed to open ""test_bucket.csv"" for reading");
        return;
    }
    String csvBucket;
    while (bucketFile.available())    {
        csvBucket += (char)bucketFile.read();
    }
    bucketFile.close();
    CSV_Parser bucket(csvBucket.c_str(),"-ssss-s");
    //bucket.print();
    RFID = (char**)bucket["RFID"];
    Visual_ID = (char**)bucket["Visual_ID"];
    NLISID = (char**)bucket["NLISID"];
    tagColour = (char**)bucket["Colour"];
    // Serial.print("Bucket File Rows: ");
    // Serial.println(bucket.getRowsCount());
    

}
void read_tags_file()   {

    File tagFile = LittleFS.open("/tags.csv");
    if(!tagFile){
        Serial.println("Failed to open ""tags.csv"" for reading");
        return;
    } else {
        Serial.println("tags.csv opened");
    }
    String csvTags;
    while(tagFile.available())  {
        csvTags += (char)tagFile.read();
    }
    tagFile.close();
    CSV_Parser allTagsList(csvTags.c_str(),"sssssuc"); 
    allTagsList.print();
    Serial.println("tags.csv closed");

}

void update_tag_status_list()   {
    // Placeholder for function to enable user to have list of tag status
    // Currently reads tag_status.csv and populates tagStaus as array
    File tagStatusFile = LittleFS.open("/tag_status.csv");
    if(!tagStatusFile){
        Serial.println("Failed to open tags_status.csv for reading");
        return;
    } else {
        Serial.println("tag_status.csv opened");
    }
    String csvStatus;
    while(tagStatusFile.available())  {
        csvStatus += (char)tagStatusFile.read();
    }
    CSV_Parser tagStatusList(csvStatus.c_str(),"s",false);
    tagStatus = (char**)tagStatusList[0];
    // for(int row = 0; row < tagStatusList.getRowsCount(); row++) {
    //     Serial.printf("Row: %i Status: %s \r\n", row, tagStatus[row]);
    // }
}

bool validateRFID(const String& inputString)    {
    if (inputString.length() != 16) {
        //not a dog by any chance?
        if(inputString == "940 110030991 001003301901") {
            //That's Redge!
            audio->connecttoFS(LittleFS, "/sounds/scanbad.mp3");
            tft.fillScreen(TFT_ORANGE);
            tft.setTextColor(TFT_BLACK);
            tft.drawCentreString("THAT'S", LV_SCREEN_HEIGHT/2, 55,4);
            tft.drawCentreString("REDGE!", LV_SCREEN_HEIGHT/2, 90,4);
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            return false;
        }
        audio->connecttoFS(LittleFS, "/sounds/scanbad.mp3");
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_BLACK);
        tft.drawCentreString("INVALID", LV_SCREEN_HEIGHT/2, 55,4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        return false;
    }
    if (inputString.substring(0, 3) != "940") {
        audio->connecttoFS(LittleFS, "/sounds/scanbad.mp3");
        tft.fillScreen(TFT_RED);
        tft.setTextColor(TFT_BLACK);
        tft.drawCentreString("INVALID", LV_SCREEN_HEIGHT/2, 55,4);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        return false;
    }
    return true;
}
