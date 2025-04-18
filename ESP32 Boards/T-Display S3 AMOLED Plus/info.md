## T-Display S3 AMOLED PLUS
This is another variant from [Lilygo](https://lilygo.cc/products/t-display-s3-amoled-plus).

This variant includes integrated sd card slot and RTC, which solves having additional modules.  It also has a power management chip. it has a good enclosure which may simplify that aspect. A carrier board will still simplify wiring. 

Support for this board is via [Lilygo's Github Repository](https://github.com/Xinyuan-LilyGO/LilyGo-AMOLED-Series).  Lewis Hu seems fairly responsive.  I have put together a [repository](https://github.com/Minorplanets76/Lilygo-T-Display-1.19-AMOLED-Minimal) for testing where I have cut out as much superfluous stuff as possible and was messing around with UI options. I have pages that cover PPM, SD Card and RTC.

For this application I am avoiding using GPIO45 & 46 due to them being strapping pins.  With the on board functionality, we are left with GPIO16, GPIO39, GPIO40, GPIO41, and GPIO42.

GPIO16 is allocated to driving the speaker.  GPIO39 allocated to the Vibrate Board, GPIO40 to LED control and  GPIO41 to Mosfet control of the RFID board power.  The RFID board is connected to UART0.