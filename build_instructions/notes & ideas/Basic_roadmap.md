#### Serial Connection with Reader Board

LilyGo T- Embed has IO43 & IO44 on grove connector.  GPIO Pins proken out are IO16 & IO17 (IO38,IO40&IO41 are all used by the sd card).

Lilygo - T-Display-S3 AMOLED Touch has IO43 & IO44 on a JST-SH 1mm pitch connector.  IO17 is used for the screen.

IO43 & IO44 are UART0

#### Making the beep
T-Embed uses IO5,6&7 but speaker has 2 wires.
T-Display has another JST connector with SDAS/SCL on IO02 & IO03.  Maybe just use a more traditional buzzer for the T-Display...

#### Storing the Scanned List as a File
https://randomnerdtutorials.com/esp32-write-data-littlefs-arduino/
https://randomnerdtutorials.com/esp32-data-logging-temperature-to-microsd-card/

Time - Can be NTP client https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/, otherwise RTC.  I have DS3231 minis.  The pinout for these are as follows 

`Pin 1 (square) = 5V or 3.3V "+"
Pin 2 = SDA "D" is data
Pin 3 = SCL "C" is clock
Pin 4 = not connected "NC" is Not Connected
Pin 5 = GND "-" is GND`


#### Sharing Files
Simplest way is probably to have a simple HTTP File Server.
https://randomnerdtutorials.com/esp32-web-server-microsd-card/

Otherwise share values with Google Sheets
https://randomnerdtutorials.com/esp32-esp8266-publish-sensor-readings-to-google-sheets/