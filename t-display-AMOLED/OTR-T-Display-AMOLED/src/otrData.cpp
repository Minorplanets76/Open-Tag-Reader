#include "otrData.h"

void BucketFile::readBucketFile() {
    //Open file
    bucket = LittleFS.open(bucketFilePath);
    if(!bucket) {
        Serial.println("Failed to open bucket file");
        return;
    }

    // Open tags.csv to compare with
    tags = LittleFS.open(tagsFilePath);
    if(!tags) {
        Serial.println("Failed to open tags file");
        return;
    }
    
    //Read Header and check in correct format
    csvBucket = bucket.readStringUntil('\n');
    if (csvBucket.isEmpty()) {
        Serial.println("Empty bucket file");
        return;
    }
    if (csvBucket != "PIC,RFID,NLISID,Visual_ID,IssueDate,ManufactureDate,Colour") {
        Serial.println("Incorrect header");
        //check if quotation marks are the issue
        csvBucket.replace("\"","");
        Serial.println(csvBucket);
        if (csvBucket != "PIC,RFID,NLISID,Visual_ID,IssueDate,ManufactureDate,Colour") {
            Serial.println("Incorrect header");
            return;
        }
       Serial.println("Correct header");
    }
    
    while (bucket.available()) {
        // Read a row
        csvBucket = bucket.readStringUntil('\n');
        rowsBucket++;
        if(checkNew())  {
            addTag();
        }
    }
    
    bucket.close();
    tags.close();
    Serial.print(rowsBucket);
    Serial.println(" rows in bucket file read");
    Serial.print(rowsAdded);
    Serial.println(" rows added to tags.csv");
    if(rowsBucket == rowsAdded) {
        Serial.println("Tags.csv successfully updated");
        LittleFS.remove(bucketFilePath);
    }

}


bool BucketFile:: checkNew() {
    //tags.csv has an added column for Status
    csvBucket.replace("\n",",UNUSED\n");
    //loop through tags.csv to check if tag is new
    tags.seek(0, SeekEnd);
    //start from the end of file (most recently added)
    while (tags.position() > 0) {
        //move the file pointer to previous row
        tags.seek(tags.position() - 1);
        // read the row
        csvTags = tags.readStringUntil('\n');
        if (csvTags == csvBucket) {
            return true;
        }
    }
    return false;
}

void BucketFile::addTag() {
    
    tags.print(csvBucket);
    tags.println();
    rowsAdded++;
}

void readLocations() {
    uint8_t numLocations = 0;
    File locationFile = LittleFS.open("/locations.csv");
    if(!locationFile) {
        Serial.println("Failed to open location file");
        return;
    }

    while (locationFile.available()) {
        // Read a row
        String locationRow = locationFile.readStringUntil('\n');
        if (numLocations == 0) {
            //Skip header row
            locationRow = locationFile.readStringUntil('\n');
        }
            //Pick out columns
        location[numLocations].name = locationRow.substring(0, locationRow.indexOf(","));
        location[numLocations].PIC = locationRow.substring(locationRow.indexOf(",")+1, locationRow.length());
        numLocations++;
    }
    Serial.print(numLocations);
    Serial.println(" locations read");
    locationFile.close();
    for (int i = 0; i < numLocations; i++) {
        Serial.print(location[i].name);
        Serial.println(location[i].PIC);
    }
}

void readTraitsFromCSV() {
    // Reads traits from csv
    // Format of file is Traitname followed by options separated by commas
    // User can define their own traits therefore no fixed dimensions
    uint8_t numTraits = 0;
    uint8_t numTraitOptions = 0;

    File traitsFile = LittleFS.open(traitsFilePath);
    if(!traitsFile) {
        Serial.println("Failed to open traits file");
        return;
    }
    while (traitsFile.available()) {
        // Read a row
        String traitRow = traitsFile.readStringUntil('\n');
        if (numTraits == 0) {
            //Skip header row
            traitRow = traitsFile.readStringUntil('\n');
        }
        String traitName = traitRow.substring(0, traitRow.indexOf(","));
        Traits newTrait;
        newTrait.traitName = traitName;
        String allTraitOptions = traitRow.substring(traitRow.indexOf(",")+1, traitRow.length());
        int optionCommaIndex;
        while ((optionCommaIndex = allTraitOptions.indexOf(",")) != -1) {
            String option = allTraitOptions.substring(0, optionCommaIndex);
            TraitOptions newOption;
            newOption.optionName = option;
            newTrait.options.push_back(newOption);           
        }
        traits.push_back(newTrait);
    }
    traitsFile.close();
}


void printTraits() {
    for (size_t i = 0; i < traits.size(); i++) {
        Serial.print("Trait: ");
        Serial.println(traits[i].traitName);
        for (size_t j = 0; j < traits[i].options.size(); j++) {
            Serial.print("  Option: ");
            Serial.println(traits[i].options[j].optionName);
        }
    }
}