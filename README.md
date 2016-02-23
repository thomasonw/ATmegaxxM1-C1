ATmegaCAN:  Arduino Core for CAN enabled Atmel AVR CPUs
========

Atmel offers a line of CPUs compatible with the CPUs commonly used in Arduino
boards, but that have an additional capability of built in CAN (Control 
Area Network) hardware.  Examples include the ATmega64M1.

ATmegaCAN is an extension to the Arduino IDE environment enabling several of these
CAN containing CPUs.

* Currently supported CPUs include:
** ATmega64M1
** ATmega32M1


Future:
  * Better menu structure
  * Inclusion of ATmega16M1, ATmega32C1, ATmega64C1
  * Clarity around the other AVR CAN CPU:  AT90CANxxx


More words to come...



Installation
------------

This is designed for Arduino 1.6.7+ 

Option 1)  Arduino Board Package Manager:
   1. Start Arduino
   2. Open menu File/Preferences
   3. Copy the following URL into the 'Additional Boards manager URLs:' box.
       * https://thomasonw.github.io/ATmegaCAN/package_thomasonw_ATmegaCAN_index.json
   4. Press OK button
   5. Open the Tools/Boards menu and select Boards Manager
   6. Scroll down and click on the 'ATmegaCAN by thomasonw version x.x.x' entery
   7. press the Install button


Option 2) Manual Install:
   1. Copy down the ZIP file to your local computer.
   2. Open the Zip File, and copy the entire content to your users local hardware subdirectory.
    * in Windows, this is: "Libraries/Documents/Arduno/Hardware"
   3. Restart the Arduino IDE




Support for IDE versions released prior to 1.6.7 maybe found at:
   http://smartmppt.blogspot.com/search/label/xxM1-IDE


