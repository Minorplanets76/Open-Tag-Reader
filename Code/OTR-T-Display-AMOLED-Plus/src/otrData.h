// otrData.h
#ifndef OTRDATA_H
#define OTRDATA_H


#include <vector>
#include <LittleFS.h>
#include "otrTime.h"

extern RTC_DS3231 rtc;


class BUCKETFILE {
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
        //char feedRowParser();
        //bool rowParserFinished();
        //CSV_Parser bucketParser;
        File bucket;
        File tags;

    private:
        char *tagID;
        bool isNew;
        bool checkNew();
        void addTag();
    
};

class ANIMALS {
    public:
        struct AnimalsFile {
            String breed;
            String type;    //can't remember why I added this.  Possibly to differentiate between stud andcommercial
            String name;
            String rfid;    //unique
            String tagged;
            bool gender;    //fixed M/F TRUE == "F"
            String mother;  //future feature
            String father;
            uint8_t multi_birth; //was the animal born as a single twin, etc
            String location;
            String group;
            String status;
            DateTime whpSafeDate;
            String comment;
        };
        String animalFilePath = "/animals.csv";
        String animalArchiveFilePath = "/archive/aminals_archive.csv";
        String animalFilePathTemp = "/backup/animals.tmp";


        ANIMALS();
        ~ANIMALS() {
            delete [] animal;
        }
        void readFile();
        void addNew(AnimalsFile);   
        void modify(AnimalsFile);
        void archive(AnimalsFile);
        void remove(AnimalsFile);
        void create();
        void renewFile();
        AnimalsFile& find(String& rfid);
        
        File animalFile;
        bool animalsFilechanged = false;
        uint16_t totalAnimals;

    
    private:
        AnimalsFile* animal;
        int numAnimals;
        
};

class RECORDS {
    public:
        struct Records {
            String index;
            String rfid; //record created when scanned
            String timeStamp;
            String location;
            String status;
            String group;
            String weight;
            String trait;  //delinineated list of traits new values eg. Udder/Dry;Feet/Bad,
            String treat; //Treat records given an index
            String comment;
        };
        String recordsFilePath = "/records.csv";
        String session;
        String lastSessions[5];
        String lastSessionFilePath = "/sessions/last_sessions.txt";
        String sessionFilePath = "session/yymmdd_1.csv";
        String sessionHeader = "Index,RFID,Timestamp,Location,Status,Group,Weight,Trait,Treat,Comment/n";
        void readFile();
        void count();
        void create();
        void addNew(Records);
        void createSession();
        String readLastSessions();
        Records* find(String& rfid, int& num);
        File recordsFile;
        bool recordsFilechanged = false;    
        uint32_t totalRecords;
        bool recordsCounted = false;

    private:
        Records* record;
        int numRecords;
        
    

};

class TREATMENTS {
    public:
        String treatmentsFilePath = "/treatments.csv";
        String productsFilePath = "/products.csv";
        int numProducts;
        int numHumans;
        struct Treatments    {
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
        struct Products    {
            String name;
            String category;
            String whp_days;
            String batchNum;
            String dosage[];
        };
        struct Humans    {
            String name;
            String initials;
        };
        void add(Treatments);
        void read();
        void create();
        void loadProducts();
        void loadHumans();
    private:
    Products* product;
    Treatments* treatment;
    Humans* human;
};

const String speciesStrings[2] = {"Sheep", "Cattle"};
const String speciesGroups[2] = {"Flock", "Herd"};
enum Species {
    Sheep,
    Cattle
};




struct TraitOptions {
    String optionName;
};
struct Traits {
    String traitName;
    std::vector<TraitOptions> options;
};


struct Transfers {
    String rfid;
    DateTime date;
    String location;
    String pic;
    String destination;
    String destinationPic;
    float price;
    String comment;
};

struct Locations {
    String name;
    String PIC;
};

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

void readLocations();
void readTraitsFromCSV();
void printTraits();

void readAnimals();
void createAnimal();
void modifyAnimal();

void changeStatus();
void changeGroup();

void changeLocation();

#endif