#include "otrFileHandling.h"
#include "LilyGo_AMOLED.h"

extern RTC_DS3231 rtc;
extern LilyGo_Class amoled;
void initFileSystem() {
    if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS"); delay(1000);
    return;
  }
  //printProgressDot();
}

void SD_init(void)
{
    bool mounted = amoled.installSD();
    lv_label_set_recolor(ui_Main_Label1, true);
    if (!mounted) {
        Serial.println("SD card installation failed");
        lv_label_set_text(ui_Main_Label1, "#ff0000 SD card \nFailed");
        return;

    } else {
        uint32_t size = SD.cardSize() / (1024 * 1024);
        uint32_t totalBytes = SD.totalBytes() / (1024 * 1024);
        uint32_t usedBytes = SD.usedBytes() / (1024 * 1024);

        Serial.printf("SD card installed. Size: %d MB, Total: %d MB, Used: %d MB\n", size, totalBytes, usedBytes);

        lv_label_set_text_fmt(ui_Main_Label1, "#00ff00 SD CARD OK \nSize: %d MB", size);
    }

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

void getStorageInfo() {
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024); //MB
    uint64_t cardUsed = SD_MMC.usedBytes() / (1024 * 1024); //MB
    uint64_t psramSize = ESP.getPsramSize() / 1024; //KB
    uint64_t flashSize = ESP.getFlashChipSize() / 1024; //KB
    uint64_t littlefsSize = LittleFS.totalBytes() / 1024; //KB
    uint64_t littlefsUsed = LittleFS.usedBytes() / 1024; //KB
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    Serial.printf(" SD Card used: %lluMB\n", cardUsed);
    Serial.printf(" psram size: %d kb\r\n", psramSize);
    Serial.printf(" FLASH size: %d kb\r\n", flashSize);
    Serial.printf(" LittleFS total: %d kb\r\n", littlefsSize);
    Serial.printf(" LittleFS used: %d kb\r\n", littlefsUsed);
}

void createExampleFile() {
  File file = SD_MMC.open("/example.txt", FILE_WRITE);
  if (file) {
    file.println("This is an example file.");
    file.println("You can add any content you want here.");
    file.close();
    Serial.println("Example file created.");
  } else {
    Serial.println("Failed to create example file.");
  }
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

String create_new_list() {
    DateTime now = rtc.now();

    String baseName = "-scans.csv";
    String currentDate = String(now.year()) + "-" + String(now.month()) + "-" + String(now.day());
    String fileName = "/" + currentDate + baseName;
    int counter = 1;

    while (LittleFS.exists(fileName.c_str())) {
        fileName = "/" + currentDate + baseName.substring(0, baseName.indexOf('.')) + "-" + String(counter) + ".csv";
        counter++;
    }

    File file = LittleFS.open(fileName.c_str(), "w");
    if (file) {
        // Add content to the file if needed
        file.close();
    }
    
    return fileName;
}