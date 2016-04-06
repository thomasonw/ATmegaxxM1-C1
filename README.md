ATmegaxxM1-C1:  Arduino Core for ATmegaxxM1/C1 CPUs
========

Atmel offers a line of CPUs compatible with the CPUs commonly used in Arduino
boards.  Some have additional features, such as the ATmega32M1 which includes built in CAN (Control 
Area Network) hardware. 

ATmegaxxM1-C1 is an extension to the Arduino IDE environment enabling several of these
CAN containing CPUs.

* Currently supported CPUs include:
 * ATmega64M1
 * ATmega32M1


Future:
  * Inclusion of ATmega16M1, ATmega32C1, ATmega64C1



Enhancements
------------

The ATmegaxxM1/C1 CPUs have some additional features which have allowed for the API to be extended.  Additions over the 'standard' Arduino are:

* Enhancement to ANALOG-READ() --> Additional ADC inputs
 * A9 will return the internal AVR temperature sensor.
    Caution:  The ADC reference MUST first be set to 2.56v via:  _analogReferance(INTERNAL);_
 * A10 will return Vcc/4
 * ADO will return results of built in differential amp AMP0 using:  (D9  - D8) * GAIN
 * AD1 will return results of built in differential amp AMP1 using:  (A4  - A3) * GAIN
 * AD2 will return results of built in differential amp AMP2 using:  (D10 - A6) * GAIN
 * _analogRead()_ of Differential amps returns a SIGNED + or - (+511 .. - 511) value depending on the relative voltages of the +&- inputs.
 * When the internal differential amps have been enabled, the existing analog port can still be read individually, however the Digital ports are disables to reduce noise.  
   
* Enhancement to PIN-MODE() --> Setting up of built in differential amps AMP0..AMP2 
 * 1st parameter selected the differential amp to enable.  Use: AD0, AD1, or AD2
 * 2nd parameter specific gain to use.  Use:  GAIN5, GAIN10, GAIN20, GAIN40
 * Example to use ADO (Ports D9 - D8):  _pinMode(AD0, GAIN20);_ 
 * To disable a differential amp and restore ports to their original use, call pinMode() with the port you wish to use normally
 * Example to restore D9 (and D8), AD0 will be turned off:  _pinMode(D9, INPUT);_ 
 
* Enhancement to  ANALOG-WRITE()  --> Access to DAC (Digital to Analog Converter)
 * A DAC is optional routed to port 10, use predefined DAC_PORT
 * writing an along value to DAC_PORT will cause the DAC to be enabled and the value used to create a analog voltage
 * DC is 10bits (just like the ADCs).  Valid values are from 0..1023
 * the DAC uses the same reference as the ADCs.  See analogReferance();
 * Example to set the DAC to 1/2 voltage:  *analogWrite(DAC_PORT, 512);*
    
 
* Change in ANALOG-REFERENCE()
 * When called using INTERNAL, a 2.56v reference is selected vs. the 1.1v on the ATmega328p
 * This is due to the hardware.

* CAN - See: <https://github.com/thomasonw/avr_can> for Arduino compatible library

   
Summary of new Keywords:  **GAIN5**, **GAIN10**, **GAIN20**, **GAIN40**, **DAC_PORT**


Notes
------------
There are some small hardware differences between the ATmegaxxM1/C1 CPUs and the ATmega328 (used in the Uno).  Mostly this will not be an issue, but one should be aware of them, including:
* The ATmegaxxM1/C1 CPUs only have two timers, TIMER0 and TIMER1.  The function _TONE_ uses the 'last timer', in this case TIMER1 (as opposed to TIMER2 in the ATmega328 / Uno) 
* The internal reference voltage is 2.56v, as opposed to 1.1v 
 



Port Mapping
------------
   
![alt text]( https://raw.githubusercontent.com/thomasonw/ATmegaxxM1-C1/master/atmega32M1%20-%20atmega64M1%20Arduino%20port%20mapping.png   "This image may be downloaded from GitHub")




 
 
Installation
------------

This is designed for Arduino 1.6.7+ 

Option 1)  Arduino Board Package Manager:
   1. Start Arduino
   2. Open menu File/Preferences
   3. Click the 'window box' to the right of the 'Additional Boards manager URLs:' entry window.
      * This will allow you to add more than one entry
   4. Copy the following URL into the box.
       * https://thomasonw.github.io/ATmegaxxM1-C1/package_thomasonw_ATmegaxxM1-C1_index.json
   5. Press OK button to close the data entry window
   6. Press OK button to close the Preferences menu.
   7. Open the Tools/Boards menu and select Boards Manager
   8. Scroll down and click on the 'ATmegaxxM1-C1 by thomasonw version x.x.x' entry
   9. press the Install button


Option 2) Manual Install:
   1. Copy down the ZIP file to your local computer.
   2. Open the Zip File, and copy the entire content to your users local hardware subdirectory.
    * in Windows, this is: "Libraries/Documents/Arduno/Hardware"
   3. Restart the Arduino IDE


After either is used to install the support files, you can select the new CPUs from the tools/boards menu.

Support for IDE versions released prior to 1.6.7 may be found at:
   http://smartmppt.blogspot.com/search/label/xxM1-IDE


   
Versions
-------------------
v1.0.3 --> Original porting using rather old CORE files.  Does NOT implement  _analogWrite()_ for DAC.  _analogRead()_ of differential ports are not corrected for +/- values (raw ADC count is returned)

v1.0.4 --> Reported to new CORE files from Arduino 1.6.8.   Added DAC support.   _analogRead()_ of differential ports are not corrected for +/- values (raw ADC count is returned)

v1.0.5 --> *pinMode(DAC_PORT,xxx)*  disconnects DAC from port (is connected by *analogWrite(DAC_PORT, xx)*.  _analogRead()_ differential ports corrected for +/- values.

v1.0.6 --> Enabled TONE capability (note, interfers with PWM as it uses the same timer..), corrected SerialWrite  IRQ race bug. 


CORE files
--------------------
A major effort for this porting effort is the modification of the CORE include files used by Arduino.  There is a separate GitHib repository which contains the edits I have made to enable the ATmegaM1/C1 CPUs:
<https://github.com/thomasonw/Arduino---CORE-files-for-ATmegaM1-C1-port/tree/master/hardware/arduino/avr/cores/arduino>

It is forked from the master Arduino files to allow updating from them as well.

