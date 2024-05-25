// otrFileHandling.h
#ifndef OTRFILEHANDLING_H
#define OTRFILEHANDLING_H

#define PIN_SD_CS             39
#define PIN_SD_SCK            40    // shared with vibrating motor
#define PIN_SD_MOSI           41
#define PIN_SD_MISO           38    // shared with power LED

#include "FS.h"
#include "LittleFS.h"
#include "SD_MMC.h"
#include <Arduino.h>
#include "time.h"
#include "Wire.h"
#include "RTClib.h"
#include "otrDisplay.h"


void initFileSystem(void);
void SD_init(void);
void copyFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, 
                                const char* destinationFilename);
void moveFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, 
                                const char* destinationFilename);
void printFileContents(const char* filePath);
void getStorageInfo(void);
void createExampleFile(void);
void appendNewLine(const char* filePath, char** bucketRow);
String create_new_list(void);





#endif