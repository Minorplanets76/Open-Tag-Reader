// otrFileHandling.h
#ifndef OTRFILEHANDLING_H
#define OTRFILEHANDLING_H



#include "FS.h"
#include "LittleFS.h"
#include "SD_MMC.h"
#include <Arduino.h>
#include "time.h"
#include "Wire.h"
#include "RTClib.h"
#include "ui/ui.h"
//#include "otrDisplay.h"


void initFileSystem(void);
void SD_init(void);
void copyFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, 
                                const char* destinationFilename);
void moveFileFromLittleFStoSD(const char* sourcePath, const char* destinationDirectory, 
                                const char* destinationFilename);
void printFileContents(String filePath);
void getStorageInfo(void);
void createExampleFile(void);
void appendNewLine(const char* filePath, char** bucketRow);
String create_new_list(void);
void listDir(const char * dirname);




#endif