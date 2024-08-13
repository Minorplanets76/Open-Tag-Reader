// otrData.h
#ifndef OTRDATA_H
#define OTRDATA_H

#include <CSV_Parser.h>
#include <vector>
#include <LittleFS.h>
#include "otrTime.h"

String traitsFilePath = "/traits.csv";
class BucketFile {
public:
    // bucket file in follwing format
    // "PIC","RFID","NLISID","Visual_ID","IssueDate","ManufactureDate","Colour"
    // "NK509384","940 110030374791","NK509384ASU00001","CHAD 001","17/11/2023","17/11/2023","BLACK"

    struct bucketFile {
        String PIC;
        String RFID;
        String NLISID;
        String Visual_ID;
        String IssueDate;
        String ManufactureDate;
        String Colour;
    };
    String bucketFilePath = "/test_bucket.csv";
    String tagsFilePath = "/tags.csv";
    String csvBucket;
    String csvTags;
    uint16_t rowsBucket;
    uint16_t rowsAdded;
    int csvBucketIndex = 0;
    int numTags = 0;
    void readBucketFile();
    char feedRowParser();
    bool rowParserFinished();
    CSV_Parser bucketParser;
    File bucket;
    File tags;

private:
    char *tagID;
    bool isNew;
    bool checkNew();
    void addTag();
    
};

struct locations {
    String name;
    String PIC;
};
locations* location;
enum Status {
    ALIVE,
    DEAD,
    SOLD
};
enum TagStatus {
    Unused,
    Active,
    Inactive
};

Status status = ALIVE;
TagStatus tagStatus = Unused;

struct tags {
    String pic;
    String rfid;
    String nlisid;
    String visual_id;
    String issueDate;
    String manufactureDate;
    String colour;
    Status status;
};

struct Animals {
    String species;
    String breed;
    String type;    //can't remember why I added this.  Possibly to differentiate between stud andcommercial
    String name;
    String rfid;    //unique
    DateTime tagged;
    bool gender;    //fixed
    String mother;  //future feature
    String father;
    uint8_t multi_birth; //was the animal born as a single twin, etc
    String location;
    String group;
    String status;
    DateTime whpSafeDate;
    String comment;
};

struct Records {
    uint32_t index;
    String rfid; //record created when scanned
    DateTime timeStamp;
    String status;
    String group;
    float weight;
    String trait;  //delinineated list of traits new values eg. Udder/Dry;Feet/Bad,
    uint32_t treat; //Treat records given an index
    String comment;
};

struct Treatment    {
    uint32_t index;
    DateTime date;
    String treatment; //eg drench, vaccinate etc
    String product; //product name
    String batch;
    String dose;
    uint8_t whp; //witholding period (days)
    String appliedBy;
    String comment;
};

struct TraitOptions {
    String optionName;
};
struct Traits {
    String traitName;
    std::vector<TraitOptions> options;
};
std::vector<Traits> traits;

struct TransfersAndSales {
    String rfid;
    DateTime date;
    String location;
    String pic;
    String destination;
    String destinationPic;
    float price;
    String comment;
};

void readLocations();
void readTraitsFromCSV();
void printTraits();

void createAnimal();
void modifyAnimal();

void changeStatus();
void changeGroup();

void changeLocation();

#endif