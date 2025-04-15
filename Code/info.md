## Code

Code is created in VScode using platformio and the Arduino framework. Keep in mind I'm an amateur without a coding background so probably aren't following best practices. Be aware I have used AI from various providers for help with coding particularly related to html/Javascript. AI has led me down quite a few dead-ends! 

There is various abandoned code still in the repository!  Beware!


### Standards
#### Conditional Compilation
Conditional compilation has been utilised to enable debugging over UART0. This also enables testing.  I've never done a project like this before so there are very likely print statements scatted throughout the code still. There is a potential conflict with the Priority 1 design RFID board which accepts serial commands to be aware of.

#### Libraries
I have created libraries appropriate to the project.  Libraries are name with otr (Open Tag Reader) as the prefix.  Currently these are:
* otrData.cpp - handles the csv data
* otrFeedback.cpp - handles the speaker, LED and Vibrate board
* otrFilehandling.cpp - probably reinvented the wheel here
* otrScanning.cpp - handles receiving and decoding of the RFID reader
* otrTime.cpp - handles things related to time and the RTC
* otrWebserver - *to be created*

#### LilyGo Libraries
LilyGo_AMOLED handles much of the the initialisation of the boards and onboard sensors and features.  Typically the code using these have been picked out of the various examples in the [Lilygo AMOLED Series Github Repository](https://github.com/Xinyuan-LilyGO/LilyGo-AMOLED-Series).

#### UI
Graphics are via the LVGL (version 8.4).
I have used Squareline software to assist with put together some of the UI. Graphics need to keep the working environment in mind, i.e. stockyards, so need to be large text and big buttons.

#### Web Interface
The onboard screen is limited so a web interface is being developed to enable the user to link up to the reader via Wifi and manipulate the data via a phone or Tablet.  This is largely created by AI due to me having no experience with HTML and Javascript.