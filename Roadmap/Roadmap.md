# Open-Tag-Reader Project Roadmap

## Project Overview
The Open-Tag-Reader project is an ESP-based RFID ear tag scanning solution designed to track and manage livestock data. It provides a user-friendly web interface for adding, updating, and reviewing animal records.

## Goals
- Create a well-structured, easy-to-maintain codebase.
- Ensure seamless RFID tag reading and data management.
- Develop a responsive web-based UI optimized for mobile use.
- Enable efficient CSV-based data storage and retrieval.
- Facilitate easy deployment and updates.

## Milestones & Tasks

### **Phase 1: Repository Cleanup & Organization**
✅ Dig up the project and restart it!

✅ Restructure file and folder layout for better navigation.

🔲 RFID information, carrier boards and code.

✅ ESP module organised into folders.

🔲 Define and document coding standards.

### **Phase 2: Core Functionality Development**
🔲 Implement reliable RFID tag scanning with ESP-based hardware.
* Serial Comms
* Handling/decoding incoming messages
* Error checking, and duplicate scan handling
* Switching RFID reader on/off

🔲 Develop a data processing system to extract and store tag details.
* tags.csv - contains tag data
* Tag Status - Unused, Active, Inactive
* bucket.csv - bucket file provided by manufacturer

🔲 Develop CSV file handling to store scan records.
* animals.csv - animal associated with tag 
* Animal Status - Alive, Dead, Sold
* records.csv
* Groups - user definable - use to count and draft
* Traits
* Locations - PIC
* Transfers - NLIS transfers and sale data
* Vetinary Chemicals - Chemical register - LPA requirement
* Treatments - Product, WHP - LPA requirement
* Humans
* Dogs

🔲 Time setting and timestamping of scan records.
* RTC integration
* Time setting manual
* Time setting NTP

🔲 Basic UI
* Session Mnagement - New, continue
* Simple Count (unique tags in a seesion)
* Advanced Count - by group, type, etc
* Settings

🔲 Scan feedback beeps and LED response.

### **Phase 3: Stick Reader Development**
🔲 RFID coil and reader board development.

🔲 ESP Board Enclosure.

🔲 Battery holder and power switch.

🔲 Implement power management for extended battery life.

🔲 Ensure ruggedness and usability in field conditions.

🔲 Test and refine the hardware and software integration.

### **Phase 4: Data Management**

🔲 Store detailed information with each animal (e.g., breed, status, age, etc.).

🔲 Allow assessment of animal traits within a session.

🔲 Develop functionality to record treatments and transfers for each animal.

### **Phase 5: Web-Based UI Development**
🔲 Create a responsive web interface for viewing and managing scanned tags.

🔲 Implement automatic field population based on scanned RFID.

🔲 Allow manual updates and additional input (e.g., gender, comments, treatments).

🔲 Design an intuitive UI with large buttons and text for usability in field conditions.

### **Phase 6: Additional Features & Enhancements**
🔲 Implement session-based scanning to track multiple animals at once.

🔲 Enable batch updates for treatments and group assignments.

🔲 Integrate filtering and search options for easier data lookup.

🔲 Add support for multiple tag formats if required.

### **Phase 7: Testing & Optimization**
🔲 Conduct extensive hardware and software testing.

🔲 Optimize performance for low-latency data retrieval and updates.

🔲 Identify and fix bugs reported by users and testers.

🔲 Implement power-saving features to enhance battery life.

🔲 Improve README with detailed project description and setup instructions.

### **Phase 8: Deployment & Community Engagement**
🔲 Create detailed deployment and setup guides.

🔲 Prepare example use cases and tutorials.

🔲 Encourage community contributions via GitHub Issues and Pull Requests.

🔲 Release a stable version with clear versioning and changelogs.

## Dependencies & Considerations
- **Hardware:** ESP-based device, RFID reader, power management.
- **Software:** Arduino framework, LittleFS/SD for storage, lightweight web server.
- **Usability:** Mobile-first design, offline usability, easy CSV integration.

## Next Steps
- [ ] Review and refine roadmap with community feedback.

- [ ] Prioritize tasks based on immediate project needs.

- [ ] Assign contributors or seek external help where necessary.

---
**Feedback & Contributions:** If you’d like to contribute, check out the `CONTRIBUTING.md` (to be created) and browse open issues on GitHub!
