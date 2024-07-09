![alt text](/images/logo.png)   
# Open-Tag-Reader  

Animal EID tag reading

Respository for Open Source Tag Reader. Built in response to [mandatory introduction](https://www.dpi.nsw.gov.au/dpi/bfs/your-role-in-biosecurity/primary-producers/nlis/eID#:~:text=NSW%20is%20currently%20transitioning%20to,Livestock%20Identification%20System%20(NLIS).) of eID tags for sheep in NSW, Australia. Intent is to develop tag readers and associated software for easy DIY.

## Stick Reader
Simple stick reader to point at ear tag to read.  Ability to store tag numbers in a scanning session, and to compare to pre-set list (eg identify culls).

Stick reader consists of a RFID reader board and a user interface. A piece of PVC electrical conduit holds it all together.  I chose that because it fits the components and my hand well.  It's bigger than it needs to be but I figured it may get misused as a whacking stick, so needs to be strong.  I haven't given too much thought to dust/water ingress at the prototype stage.  I'm experimenting with different variants, including extendable versions.

### RFID Reader Board
I have tested two differnt reader solutions and both seem to work OK, but need extensive testing.

The first is an Australian product from  [Priority 1 Design](https://www.priority1design.com.au/) who have designed a number of RFID reading boards including a number for reading [Animal Tags](https://www.priority1design.com.au/rfid_reader_modules.html#animal_tag_reader_writer).  The board used is the [FDX-B/HDX RFID Reader Writer with TTL serial port](https://www.priority1design.com.au/shopfront/index.php?main_page=product_info&cPath=1&products_id=10) as this reads the HDX format required and is simple to interface to another microcontroller operating the User Interface.  The board has the option to use different antennas, which will affect range, but comes with a 49mm coil that is is easy to implement.  It simply blurts out the Country Code and Tag ID in ASCII format over serial interface.  It also has the ability to receive commands via serial.

The second is the WL-134A board that I found on [Aliexpress](https://www.aliexpress.com/item/1005005527774729.html?spm=a2g0o.detail.pcDetailTopMoreOtherSeller.5.28f8USkxUSkxHo&gps-id=pcDetailTopMoreOtherSeller&scm=1007.40050.354490.0&scm_id=1007.40050.354490.0&scm-url=1007.40050.354490.0&pvid=1610bac3-e44d-44ca-a5d6-faea2dcb6218&_t=gps-id:pcDetailTopMoreOtherSeller,scm-url:1007.40050.354490.0,pvid:1610bac3-e44d-44ca-a5d6-faea2dcb6218,tpp_buckets:668%232846%238107%231934&pdp_npi=4%40dis%21AUD%2145.81%2138.49%21%21%2130.46%2125.59%21%402101ef7017204727855125053e5b6c%2112000033424318969%21rec%21AU%21136852735%21&utparam-url=scene%3ApcDetailTopMoreOtherSeller%7Cquery_from%3A).  This board also works fine, but was just a little more challenging to interface as it sends the entire 64bytes of the tag through serial.  It's quite a bit smaller, so is a bit more convenient to squeeze into smaller diameter conduit.  I designed up a carrier board for it to simplify the wiring.  It comes wil a 70mm and 150mm coil and a capicitor to suit.

#### Reader Board Carrier and Antenna Holder
The antenna wire is diabolically fine, so I designed up a carrier that holds the board and antenna, ensuring things don't move around, and fits to the chosen conduit.

### User Interface
As is often the case, the software has suffered some feature creep! I have been trialing ESP32 and screen combinations from [lilygo.cc](https://www.lilygo.cc/) namely the [T-Embed](https://www.lilygo.cc/products/t-embed) and the [T-Display AMOLED Touch](https://www.lilygo.cc/products/t-display-s3-amoled). They have their pros and cons, so I have made two versions and am testing to see how they fair in the sheepyards.  Everything has/is been put together through PlatformIO and VScode with the Arduino framework.  This has been a learning experience for me.

Basic functionality is to scan tags and add to lists, or scan and indicate if on the selected list.  I also plan to add treatment and trait information.  I think I will expand the interface so that a lot of this can be done through a tablet or phone via a wifi access point.

#### Feedback
A small speaker, a vibrating motor, and a switch with integrated led have been incorporated into the design to give the user feed back from the scan.

#### Power
Power is from a single cell lipo battery.  The main switch cut's off power when not in use.  A small boost converter provides 9V to the reader board.  Mosfets allow the reader board to switched on and of, the battery to be charged when plugged into USB, and ensures the speaker is as loud as possible.

### Build Instructions
See [build instructions](/build_instructions/build_instructions.md)

# Further Information
https://www.integritysystems.com.au/siteassets/nlis-rfid-standards.pdf
