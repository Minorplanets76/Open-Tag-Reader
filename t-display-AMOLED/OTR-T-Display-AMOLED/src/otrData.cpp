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

Animals::Animals()  {
    numAnimals = 0;
    totalAnimals = 0;
}
    
Animals::~Animals() {
    delete[] animal;
}
void Animals::readFile() {
    animalFilePath = "/" + speciesStrings[species] + "/" + speciesGroups[species] + ".csv";
    animalFile = LittleFS.open(animalFilePath);
    if (!animalFile) {
        Serial.print("Failed to open file: ");
        Serial.println(animalFilePath);
        return;
    }
    animal = nullptr; // Initialize the class member animal to nullptr
    animalFile.readStringUntil('\n'); // skip header
    String animalRow;
    while (animalFile.available()) {
        // Read a row
        String animalRow = animalFile.readStringUntil('\n');
        totalAnimals++;
        // Pick out columns
        int commaIndex = 0;
        String columns[14];
        for (int i = 0; i < 14; i++) {
        commaIndex = animalRow.indexOf(",", commaIndex);
        if (commaIndex == -1) {
            columns[i] = animalRow.substring(commaIndex + 1);
            break;
        }
        columns[i] = animalRow.substring(0, commaIndex);
        animalRow = animalRow.substring(commaIndex + 1);
        }
        

        animal = (AnimalsFile*)realloc(animal, (numAnimals + 1) * sizeof(AnimalsFile));
        animal[numAnimals].breed = columns[0];
        animal[numAnimals].type = columns[1];
        animal[numAnimals].name = columns[2];
        animal[numAnimals].rfid = columns[3];
        animal[numAnimals].tagged = stringToDateTime(columns[4]);
        if (columns[5] == "F") {
        animal[numAnimals].gender = true;
        }
        else {
        animal[numAnimals].gender = false;
        }
        animal[numAnimals].mother = columns[6];
        animal[numAnimals].father = columns[7];
        animal[numAnimals].multi_birth = columns[8].toInt();
        animal[numAnimals].location = columns[9];
        animal[numAnimals].group = columns[10];
        animal[numAnimals].status = columns[11];
        animal[numAnimals].whpSafeDate = DateTime(columns[12].toInt());
        animal[numAnimals].comment = columns[13];
        numAnimals++; // Move the increment to the end
    }
    animalFile.close();
}
void Animals::addNew(AnimalsFile newAnimal) {
    //readAnimalFile() must have already run
    animal = (AnimalsFile*)realloc(animal, (numAnimals + 1) * sizeof(AnimalsFile));
    animal[numAnimals] = newAnimal;
    numAnimals++;
    //append to file
    File animalFile = LittleFS.open(animalFilePath, FILE_APPEND);
    if (!animalFile) {
        Serial.println("Failed to open animal file");
        return;
    }
    
    animalFile.print(newAnimal.breed + ",");
    animalFile.print(newAnimal.type + ",");
    animalFile.print(newAnimal.name + ",");
    animalFile.print(newAnimal.rfid + ",");
    animalFile.print(dateTimeToString(newAnimal.tagged) + ",");
    animalFile.print((newAnimal.gender ? "F," : "M,") );
    animalFile.print(newAnimal.mother + ",");
    animalFile.print(newAnimal.father + ",");
    animalFile.print(newAnimal.multi_birth + ",");
    animalFile.print(newAnimal.location + ",");
    animalFile.print(newAnimal.group + ",");
    animalFile.print(newAnimal.status + ",");
    animalFile.print(dateTimeToString(newAnimal.whpSafeDate) + ",");
    animalFile.println(newAnimal.comment);
    animalFile.close();
}
void Animals::modify(AnimalsFile updatedAnimal) {   
        for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == updatedAnimal.rfid) {
            animal[i] = updatedAnimal;
            animalsFilechanged = true;
            return;
        }
    }
    throw std::runtime_error("Animal not found");
}

void Animals::archive(AnimalsFile animalToRemove) {   
    animalArchiveFilePath = "/" + speciesStrings[species] + "/archive/" + speciesGroups[species] + "_archive.csv";
    File animalArchiveFile = LittleFS.open(animalArchiveFilePath, FILE_APPEND);
    if (!animalArchiveFile) {
        Serial.println("Failed to open animal archive file");
        return;
    }
    
    animalArchiveFile.print(animalToRemove.breed + ",");
    animalArchiveFile.print(animalToRemove.type + ",");
    animalArchiveFile.print(animalToRemove.name + ",");
    animalArchiveFile.print(animalToRemove.rfid + ",");
    animalArchiveFile.print(dateTimeToString(animalToRemove.tagged) + ",");
    animalArchiveFile.print((animalToRemove.gender ? "F," : "M,") );
    animalArchiveFile.print(animalToRemove.mother + ",");
    animalArchiveFile.print(animalToRemove.father + ",");
    animalArchiveFile.print(animalToRemove.multi_birth + ",");
    animalArchiveFile.print(animalToRemove.location + ","); 
    animalArchiveFile.print(animalToRemove.group + ",");
    animalArchiveFile.print(animalToRemove.status + ",");
    animalArchiveFile.print(dateTimeToString(animalToRemove.whpSafeDate) + ",");
    animalArchiveFile.println(animalToRemove.comment);
    animalArchiveFile.close();
}
void Animals::remove(AnimalsFile animalToRemove) {
    for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == animalToRemove.rfid) {
            // Shift all elements after the removed element to the left
            for (int j = i; j < numAnimals - 1; j++) {
                animal[j] = animal[j + 1];
            }
            numAnimals--;
            animalsFilechanged = true;
            return;
        }
    }
    throw std::runtime_error("Animal not found");
}

Animals::AnimalsFile& Animals::find(const String& rfid) {
    for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == rfid) {
            return animal[i];
        }
    }
    throw std::runtime_error("Animal not found");
}

void Animals::create() {

}

void Animals::renewFile() {
    //copy header
    File currentFile =LittleFS.open(animalFilePath, "r");
    if (!currentFile) {
        Serial.println("Failed to open animal file");
        return;
    }
    String header ="";
    if (currentFile.available()) {
        header = currentFile.readStringUntil('\n');
    }
    currentFile.close();
    //rename file
    animalFilePathTemp = animalFilePath;
    animalFilePathTemp.replace(".csv", ".tmp");
    if (!LittleFS.rename(animalFilePath, animalFilePathTemp)) {
        Serial.println("Failed to rename animal file");
        return;
    }
    //create new file
    File newFile = LittleFS.open(animalFilePath, FILE_WRITE);
    if (!newFile) {
        Serial.println("Failed to create new animal file");
        return;
    }
    newFile.println(header);
    //copy animals to new file
    for (int i = 0; i < numAnimals; i++) {
        newFile.print(animal[i].breed + ",");
        newFile.print(animal[i].type + ",");
        newFile.print(animal[i].name + ",");
        newFile.print(animal[i].rfid + ",");
        newFile.print(dateTimeToString(animal[i].tagged) + ",");
        newFile.print((animal[i].gender ? "F," : "M,") );
        newFile.print(animal[i].mother + ",");
        newFile.print(animal[i].father + ",");
        newFile.print(animal[i].multi_birth + ",");
        newFile.print(animal[i].location + ",");
        newFile.print(animal[i].group + ",");
        newFile.print(animal[i].status + ",");
        newFile.print(dateTimeToString(animal[i].whpSafeDate) + ",");
        newFile.println(animal[i].comment);
    }
    newFile.close();
    
}