# Building Open Tag Reader
## Bill of Materials
|Component|Qty| Source |
|---------|:---:|--------|
|Tag reader board|1|[FDX-B/HDX RFID Reader Writer with TTL serial port](https://www.priority1design.com.au/shopfront/index.php?main_page=product_info&cPath=1&products_id=10)|
Right angle header|1|  |
|Reader/Antenna Carrier|1|[3D Print - Coil Reader](/3d_print_files/)|
|Shroud|1|[3D Print - Coil Reader Shroud](/3d_print_files/)|
|LilyGo Embed|1|[T-Embed](https://www.lilygo.cc/products/t-embed)|
|Connecting wire|1|Ribbon cable and Dupont connectors|
|LiPo Battery|1| |
|Battery lead|1|JST PH2.0 Female|
|Boost converter|1| |
|JST Y Cable|1|  |
|USB connector| 1| |
|Battery Holder|1|[3D Print](/3d_print_files/)|
|Screen adapter|1|[3D Print](/3d_print_files/)|
|End cap|1|[3D Print](/3d_print_files/)|

## Prepare Reader Board
1. Solder right angle header.
2. Attach to carrier board.


## Attach antenna
1. Seat antenna in groove aligning pigtails with the slots.
2. Feed under and through board at A+ and A-
3. Check wires seated and fix in place.
4. Solder antenna to board.

## Flash Firmware

## Prepare Pipe
1. Drill holes for Shroud/carrier board
2. Make slot for display.

## Prepare T-Enbed
1. Solder header

## Wiring
The battery is a different JST connector to the T-Embed Board.  A boost converter is required for the reader board. A female JST PH2.0 socket and tail is recommended so the battery can be disconnected, but the connector on the battery can be cut off and the battery soldered directly.
1. Solder the Battery and Board pigtails to the Boost Converter board V*in* pads
2. Solder headers to V*out* pads
3. Make connecting cable with JST connectors either side.