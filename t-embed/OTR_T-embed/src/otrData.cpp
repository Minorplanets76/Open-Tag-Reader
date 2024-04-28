#include "otrData.h"

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


void read_tag_status_list()   {
    // Placeholder for function to enable user to have list of tag Groups
    // Currently reads tag_status.csv and populates groups as array
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


void read_active_tags_file()   {

}

