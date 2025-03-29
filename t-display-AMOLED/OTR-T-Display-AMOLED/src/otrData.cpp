#include "otrData.h"
#include "otrFileHandling.h"
extern Species species;

void BUCKETFILE::readBucketFile() {
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
        //check if quotation marks are the issue
        csvBucket.replace("\"","");
        Serial.println(csvBucket);
        if (csvBucket != "PIC,RFID,NLISID,Visual_ID,IssueDate,ManufactureDate,Colour") {
            Serial.println("Incorrect header");
            return;
        }
        //Serial.println("Correct header");
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


bool BUCKETFILE::checkNew() {
  //tags.csv has an added column for Status
  csvBucket.replace("\n",",UNUSED\n");
  tags.seek(1, SeekEnd);//start from the end of file (most recently added) but before /n
  char c;
  int startPosition;
  while (tags.position() > 0) {
    c = tags.read();
    if (c == '\n') {
        startPosition = tags.position()-1;
        csvTags = tags.readStringUntil('\n');
        if (csvTags == csvBucket) {
            return true;
        }
        tags.seek(startPosition);
    }
    tags.seek(tags.position() - 1);
  }
  return false;
}

void BUCKETFILE::addTag() {
    tags.print(csvBucket);
    tags.print(",Unsed");
    tags.println();
    rowsAdded++;
}

void readLocations() {
    uint8_t numLocations = 0;
    Locations* location = nullptr;
    File locationFile = LittleFS.open("/locations.csv");
    if (locationFile) {
        while (locationFile.available()) {
            locationFile.readStringUntil('\n');
            numLocations++;
        }
        locationFile.seek(0); // move back to the beginning of the file
        location = new Locations[numLocations-1]; // allocate memory for the array
        for (uint8_t i = 0; i < numLocations; i++) {
            String locationRow = locationFile.readStringUntil('\n');
            if (i == 0) {
                // Skip header row
                continue;
            }
            // Pick out columns
            location[i].name = locationRow.substring(0, locationRow.indexOf(","));
            location[i].PIC = locationRow.substring(locationRow.indexOf(",") + 1, locationRow.length());
        }
        locationFile.close();
    }
    
}
//INCOMPLETE
void readTraitsFromCSV() {
    // Reads traits from csv
    // Format of file is Traitname followed by options separated by commas
    // User can define their own traits therefore no fixed dimensions
    // uint8_t numTraits = 0;
    // Traits* trait = nullptr;
    // File traitsFile = LittleFS.open(traitsFilePath);

    // if (traitsFile) {
    //     while (traitsFile.available()) {
    //         traitsFile.readStringUntil('\n');
    //         numTraits++;
    //     }
    //     traitsFile.seek(0); // move back to the beginning of the file
    //     trait = new Traits[numTraits-1]; // allocate memory for the array
    //     while (traitsFile.available()) {
    //         String traitRow = traitsFile.readStringUntil('\n');
    //         if (numTraits == 0) {
    //             // Skip header row
    //             continue;
    //         }
    //         // Pick out columns
    //         trait[numTraits-1].traitName = traitRow.substring(0, traitRow.indexOf(","));
    //         String allTraitOptions = traitRow.substring(traitRow.indexOf(",") + 1, traitRow.length());
    //         int optionCommaIndex;
    //         while ((optionCommaIndex = allTraitOptions.indexOf(",")) != -1) {
    //             String option = allTraitOptions.substring(0, optionCommaIndex);
    //             trait[numTraits-1].options.push_back(option);
    //             allTraitOptions = allTraitOptions.substring(optionCommaIndex + 1, allTraitOptions.length());
    //         }
    //         // Add the last option
    //         trait[numTraits-1].options.push_back(allTraitOptions);
    //     }
    // }
 
    // while (traitsFile.available()) {
    //     // Read a row
    //     String traitRow = traitsFile.readStringUntil('\n');
    //     if (numTraits == 0) {
    //         // Skip header row
    //         traitRow = traitsFile.readStringUntil('\n');
    //     }
    //     String traitName = traitRow.substring(0, traitRow.indexOf(","));
    //     Traits newTrait;
    //     newTrait.traitName = traitName;
    //     String allTraitOptions = traitRow.substring(traitRow.indexOf(",") + 1, traitRow.length());
    //     int optionCommaIndex;
    //     while ((optionCommaIndex = allTraitOptions.indexOf(",")) != -1) {
    //         String option = allTraitOptions.substring(0, optionCommaIndex);
    //         TraitOptions newOption;
    //         newOption.optionName = option;
    //         newTrait.options.push_back(newOption);
    //         allTraitOptions = allTraitOptions.substring(optionCommaIndex + 1, allTraitOptions.length());
    //     }
    //     // Add the last option
    //     TraitOptions newOption;
    //     newOption.optionName = allTraitOptions;
    //     newTrait.options.push_back(newOption);
    //     // Add the trait to the traits vector
    //     traits.push_back(newTrait);
    //     numTraits++;
    // }
    // traitsFile.close();
}

//Not working
void printTraits() {
    // for (size_t i = 0; i < traits.size(); i++) {
    //     Serial.print("Trait: ");
    //     Serial.println(traits[i].traitName);
    //     for (size_t j = 0; j < traits[i].options.size(); j++) {
    //         Serial.print("  Option: ");
    //         Serial.println(traits[i].options[j].optionName);
    //     }
    // }
}

ANIMALS::ANIMALS()  {
    numAnimals = 0;
    totalAnimals = 0;
}
    
void ANIMALS::readFile() {
    Species species = Sheep;
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
        animal[numAnimals].tagged = columns[4];
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
void ANIMALS::addNew(AnimalsFile newAnimal) {
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
    animalFile.print(newAnimal.tagged + ",");
    animalFile.print((newAnimal.gender ? "F," : "M,") );
    animalFile.print(newAnimal.mother + ",");
    animalFile.print(newAnimal.father + ",");
    animalFile.print(newAnimal.multi_birth + ",");
    animalFile.print(newAnimal.location + ",");
    animalFile.print(newAnimal.group + ",");
    animalFile.print(newAnimal.status + ",");
    animalFile.print(dateToString(newAnimal.whpSafeDate) + ",");
    animalFile.println(newAnimal.comment);
    animalFile.close();
}

void ANIMALS::modify(AnimalsFile updatedAnimal) {   
    for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == updatedAnimal.rfid) {
            animal[i] = updatedAnimal;
            animalsFilechanged = true;
            return;
        }
    }
}

void ANIMALS::archive(AnimalsFile animalToRemove) {  
    Species species = Sheep; 
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
    animalArchiveFile.print(animalToRemove.tagged + ",");
    animalArchiveFile.print((animalToRemove.gender ? "F," : "M,") );
    animalArchiveFile.print(animalToRemove.mother + ",");
    animalArchiveFile.print(animalToRemove.father + ",");
    animalArchiveFile.print(animalToRemove.multi_birth + ",");
    animalArchiveFile.print(animalToRemove.location + ","); 
    animalArchiveFile.print(animalToRemove.group + ",");
    animalArchiveFile.print(animalToRemove.status + ",");
    animalArchiveFile.print(dateToString(animalToRemove.whpSafeDate) + ",");
    animalArchiveFile.println(animalToRemove.comment);
    animalArchiveFile.close();
}

void ANIMALS::remove(AnimalsFile animalToRemove) {
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
}

ANIMALS::AnimalsFile& ANIMALS::find(String& rfid) {
    for (int i = 0; i < numAnimals; i++) {
        if (animal[i].rfid == rfid) {
            return animal[i];
        }
    }
    throw std::runtime_error("Animal not found");
}
//UNFINISHED
void ANIMALS::create() {
    //pull data from ui
}

void ANIMALS::renewFile() {
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
        newFile.print(animal[i].tagged + ",");
        newFile.print((animal[i].gender ? "F," : "M,") );
        newFile.print(animal[i].mother + ",");
        newFile.print(animal[i].father + ",");
        newFile.print(animal[i].multi_birth + ",");
        newFile.print(animal[i].location + ",");
        newFile.print(animal[i].group + ",");
        newFile.print(animal[i].status + ",");
        newFile.print(dateToString(animal[i].whpSafeDate) + ",");
        newFile.println(animal[i].comment);
    }
    newFile.close();
}
//UNFINISHED
void RECORDS::readFile() {
    Species species = Sheep;
    recordsFilePath = "/" + speciesStrings[species] + "/records.csv";
    recordsFile = LittleFS.open(recordsFilePath, "r");
    if (!recordsFile) {
        Serial.println("Failed to open records file");
        return;
    }
    String header = recordsFile.readStringUntil('\n');
    
    recordsFile.close();
}

void RECORDS::count() {
    recordsFile = LittleFS.open(recordsFilePath, "r");
    recordsFile.seek(0, SeekEnd);
    recordsFile.seek(recordsFile.position() - 1);
    totalRecords = recordsFile.readStringUntil(',').toInt();
    recordsFile.close();
    recordsCounted = true;
}
//UNFINISHED
void RECORDS::create() {
    //pull data from ui
}

void RECORDS::addNew(Records newRecord) {
    recordsFile = LittleFS.open(recordsFilePath, FILE_APPEND);
    if (!recordsFile) {
        Serial.println("Failed to open records file");
        return;
    }
    recordsFile.seek(0, SeekEnd); //go to end of file
    if (!recordsCounted) {//count function must be run prior
        count();
    }
    totalRecords++; 
    recordsFile.print(totalRecords);
    recordsFile.print("," + newRecord.rfid + ",");
    recordsFile.print(newRecord.timeStamp + ",");
    recordsFile.print(newRecord.location + ",");
    recordsFile.print(newRecord.status + ",");
    recordsFile.print(newRecord.group + ",");
    recordsFile.print(newRecord.weight + ","); //placeholder for weight
    recordsFile.print(newRecord.trait + ",");
    recordsFile.print(String(newRecord.treat) + ",");
    recordsFile.println(newRecord.comment);
    recordsFile.close();
}

void RECORDS::createSession() {
    //session name is current date with sequential number yyyymmdd_1, yyyymmdd_2 etc
    //in order to continue a session after power down etc last 5 sessions stored in last_sessions.txt

    DateTime dt = rtc.now();
    String dateStr = dateToSessionFormat(dt);
    readLastSessions();
    Serial.println(dateStr);
    Serial.println(lastSessions[4].substring(0, 6));
    if(lastSessions[4].substring(0, 6) == dateStr) {
        int num = lastSessions[4].substring(7).toInt();
        session = dateStr + "_" + String(num + 1);
    } else {
        session = dateStr + "_1";
    }
    String newSessions[5];
    for (int i = 1; i < 5; i++) {
        newSessions[i-1] = lastSessions[i];
    }
    newSessions[4] = session;
    if (!LittleFS.remove(lastSessionFilePath)) {
        Serial.println("Failed to remove last sessions file");
        return;
    }
    File file = LittleFS.open(lastSessionFilePath, "w");
     if (!file) {
        Serial.println("Failed to create last sessions file");
        return;
    }

    for (int i = 0; i < 5; i++) {
        file.println(newSessions[i]);
    }
  
    file.close();
    sessionFilePath = "/" + speciesStrings[species] + "/sessions/" + session + ".csv";
    File sessionFile = LittleFS.open(sessionFilePath, "w");
    if (!sessionFile) {
        Serial.println("Failed to create session file");
        return;
    }
    sessionFile.println(sessionHeader);
    recordsFilePath = sessionFilePath;
    numRecords = 0;
    recordsCounted = true;
    sessionFile.close();
}

String RECORDS::readLastSessions() {
    
    lastSessionFilePath = "/" + speciesStrings[species] + "/sessions/last_sessions.txt";
    
    String sessionsDropdown;

    File file = LittleFS.open(lastSessionFilePath, "r");
    if (!file) {
        // Create the file if it doesn't exist
        file = LittleFS.open(lastSessionFilePath, "w");
        if (!file) {
            Serial.println("Failed to create last sessions file");
            return "error";
        }
        // Initialize the file with empty lines
        for (int i = 0; i < 5; i++) {
            file.println();
        }
        file.close();
        // Reopen the file for reading
        file = LittleFS.open(lastSessionFilePath, "r");
    }

    int i = 0;
    while (file.available() && i < 5) {
        String line = file.readStringUntil('\n');
        lastSessions[i] = line;
        sessionsDropdown = sessionsDropdown + line + '\n';
        i++;
    }
    file.close();
    sessionsDropdown = sessionsDropdown.substring(0, sessionsDropdown.length() - 1);
    return sessionsDropdown;
    
}
RECORDS::Records* RECORDS::find(String& rfid, int& num) {
    recordsFile = LittleFS.open(recordsFilePath, "r");
    String csvRecord = recordsFile.readStringUntil('\n'); //1st line
    Records* thisRfid;
    num = 0;
    while (recordsFile.available()) {
        csvRecord = recordsFile.readStringUntil('\n');
        int fileIndex = csvRecord.indexOf(rfid);
        if (fileIndex != -1) {
            // Pick out columns
            int commaIndex = 0;
            String columns[10];
            for (int i = 0; i < 10; i++) {
                if (commaIndex == -1) {
                    columns[i] = csvRecord.substring(commaIndex + 1);
                    break;
                }
                else {
                    columns[i] =csvRecord.substring(commaIndex, csvRecord.indexOf(',', commaIndex));
                    commaIndex = csvRecord.indexOf(',', commaIndex + 1);
                }
            }
            thisRfid[num].index = columns[0].toInt();
            thisRfid[num].rfid = columns[1];
            thisRfid[num].timeStamp = columns[2];
            thisRfid[num].location = columns[3];
            thisRfid[num].status = columns[4];
            thisRfid[num].group = columns[5];
            thisRfid[num].weight = columns[6].toFloat();
            thisRfid[num].trait = columns[7];
            thisRfid[num].treat = columns[8].toInt();
            thisRfid[num].comment = columns[9];
            num++;
        }
    }
    recordsFile.close();
    return thisRfid;
    
}
//UNFINISHED
void TREATMENTS::add(Treatments newTreatment) {

}
//UNFINISHED
void TREATMENTS::read() {

}
//UNFINISHED
void TREATMENTS::create() {
    //pull data from ui
    
}

void TREATMENTS::loadProducts() {
    Species species = Sheep;
    productsFilePath = "/" + speciesStrings[species] + "/treatments/products.csv";
    File productsFile = LittleFS.open(productsFilePath, "r");
    if (!productsFile) {
        Serial.println("Failed to open products file");
        return;
    }

    String header = productsFile.readStringUntil('\n');
    numProducts = 0;
    

    // Read each row and allocate memory for the product struct
    while (productsFile.available()) {
        String productRow = productsFile.readStringUntil('\n');
        int columnNum = 0;
        int startIndex = 0;
        String columns[20];
        int endIndex = productRow.indexOf(',');
        while (endIndex != -1) {
            String column = productRow.substring(startIndex, endIndex);
            startIndex = endIndex + 1;
            endIndex = productRow.indexOf(',', startIndex);
            columns[columnNum] = column;
            columnNum++;           
        }

        product[numProducts].name = columns[0];
        product[numProducts].category = columns[1];
        product[numProducts].whp_days = columns[2];
        product[numProducts].batchNum = columns[3];
        int numDosages = columnNum - 4;
        for (int i = 0; i < numDosages; i++) {
            product[numProducts].dosage[i] = columns[i + 4];
        }
        numProducts++;
    }

    productsFile.close();
}

void TREATMENTS::loadHumans() {
    
    File humansFile = LittleFS.open("/humans.csv", "r");
    if (!humansFile) {
        Serial.println("Failed to open humans file");
        return;
    }
    numHumans = 0;
    String header = humansFile.readStringUntil('\n');
    while (humansFile.available()) {
        String humansRow = humansFile.readStringUntil('\n');
        human[numHumans].name = humansRow.substring(0, humansRow.indexOf(','));
        human[numHumans].initials = humansRow.substring(humansRow.indexOf(',') + 1);
        numHumans++;
    }

    humansFile.close();

}

