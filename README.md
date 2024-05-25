![alt text](/images/logo.png)   
# Open-Tag-Reader  

Animal EID tag reading

Respository for Open Source Tag Reader. Built in response to [mandatory introduction](https://www.dpi.nsw.gov.au/dpi/bfs/your-role-in-biosecurity/primary-producers/nlis/eID#:~:text=NSW%20is%20currently%20transitioning%20to,Livestock%20Identification%20System%20(NLIS).) of eID tags for sheep in NSW, Australia. Intent is to develop tag readers and associated software for easy DIY.

## Stick Reader
Simple stick reader to point at ear tag to read.  Ability to store tag numbers in a scanning session, and to compare to pre-set list (eg identify culls).

Stick reader consists of a RFID reader board and a user interface. A piece of PVC water pipe (40mm high pressure) holds it all together.  I chose that because it fits the components and my hand well.  It's bigger than it needs to be but I figured it may get misused as a whacking stick, so needs to be strong.  I haven't given too much thought to dust/water ingress at the prototype stage.

### RFID Reader Board
I chose an Australian product from  [Priority 1 Design](https://www.priority1design.com.au/) who have designed a number of RFID reading boards including a number for reading [Animal Tags](https://www.priority1design.com.au/rfid_reader_modules.html#animal_tag_reader_writer).  The board used is the [FDX-B/HDX RFID Reader Writer with TTL serial port](https://www.priority1design.com.au/shopfront/index.php?main_page=product_info&cPath=1&products_id=10) as this reads the HDX format required and is simple to interface to another microcontroller operating the User Interface.  The board has the option to use different antennas, which will affect range, but comes with a small coil that is is easy to implement.

#### Board Carrier and Antenna Holder
The board fits inside the pipe but the standard 49mm antenna is a bit big.  The antenna wire is diabolically fine, so I designed up a carrier that holds the board and antenna, whilst also acting as a soldering jig for the fine antenna wires to A+ and A-.

### User Interface
As is often the case, the software has suffered some feature creep! I have been trialing ESP32 and screen combinations from [lilygo.cc](https://www.lilygo.cc/) namely the [T-Embed](https://www.lilygo.cc/products/t-embed) and the [T-Display AMOLED Touch](https://www.lilygo.cc/products/t-display-s3-amoled). They have their pros and cons, so I have made two versions and am testing to see how they fair in the sheepyards.  Everything has/is been put together through PlatformIO and VScode with the Arduino framework.  This has been a learning experience for me.




### Build Instructions
See [build instructions](/build_instructions/build_instructions.md)

# Further Information
https://www.integritysystems.com.au/siteassets/nlis-rfid-standards.pdf
