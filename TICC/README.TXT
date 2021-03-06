Software for the TAPR TICC timestamping counter.

This is an Arduino sketch.  To build, unpack all the files into
a folder named TICC and point the Arduino IDE at it.  The TICC shield
(and this software) work with an Arduino Mega 2560 board.

You will need to install the EnableInterrupts library.  A .zip file is
included in the package.  From the IDE, go to

     Sketches/Include Library/Add .ZIP Library

and point to EnableInterrupts.zip.  Click Install and the library will be
installed to the IDE library directory.

HARDWARE SETUP
The TICC is powered by the USB cable from its host computer.  It requires
a 10 MHz clock signal connected to the "REF IN" SMA jack.  Test signals
are applied to the SMA jacks for channels A and B.

TALKING TO THE TICC
The Arduino communicates with a host computer via a USB serial port.
Linux and Windows 10 have appropriate USB drivers built in.  For Windows
7 and earlier, you will need to download and install drivers.  See the
User's Manual for instructions.

Serial comms with the TICC are at 115200 N81.  All data is 7 bit ASCII.

OTHER CONFIGURATION
Nearly all of the variables that might want configuration are in a
struct that's cleverly called "config".  Calling the function
eeprom_write_config_default() will write default values into the
Arduino EEPROM.  The first byte of the struct is a version number.
At startup, the sketch reads byte 0 of the eeprom and if the version
number does not match, calls eeprom_write_config_default().  Then,
the sketch reads the configuration back into the config struct for use
by the program.

Ultimately the startup config menu will allow you to set any of the
variables.

John Ackermann   N8UR
jra@febo.com
16 February 2017

