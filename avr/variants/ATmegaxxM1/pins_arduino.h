/*
  pins_arduino.h - Pin definition functions for Arduino
  Part of Arduino - http://www.arduino.cc/


  Modified to support ATmega32M1, ATmega64M1, etc.   Feb 2015, 2016 
        Al Thomason:  http://smartmppt.blogspot.com/




  Copyright (c) 2007 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 249 2007-02-03 16:52:51Z mellis $
*/

#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <avr/pgmspace.h>

#define NUM_DIGITAL_PINS            24
#define NUM_ANALOG_INPUTS            9

#define analogInputToDigitalPin(p)  ((p < 9) ? (p) + 15 : -1)
#define digitalPinHasPWM(p)         ((p) == 1 || (p) == 2 || (p) == 14)

//      __AVR_ATmega32M1__  & __AVR_ATmega64M1__ has an unusual mapping of pins to channels and needs extra lookup table
extern const uint8_t PROGMEM analog_pin_to_channel_PGM[];
#define analogPinToChannel(P)  ( pgm_read_byte( analog_pin_to_channel_PGM + (P) ) )




static const uint8_t SS   =  1;
static const uint8_t MISO =  5;
static const uint8_t MOSI =  6;
static const uint8_t SCK  = 23; 

//static const uint8_t LED_BUILTIN = 13;                // Have disabled 'built in' LED in default optiboot compiled for ATmegaxxM1 - allows unrestricted use of that port.

static const uint8_t A0 =  15;                          // Mapping Ax referance to "port#"
static const uint8_t A1 =  16;  
static const uint8_t A2 =  17;
static const uint8_t A3 =  18;
static const uint8_t A4 =  19;
static const uint8_t A5 =  20;
static const uint8_t A6 =  21;
static const uint8_t A7 =  22;
static const uint8_t A8 =  23;
                                                                // 'Virtual' analog ports - 
static const uint8_t A9 =  24;                                  // Internal core temp sensor
static const uint8_t A10 = 25;                                  // Internal Vcc / 4
static const uint8_t AD0 = 26;                                  // Differential AMP0
static const uint8_t AD1 = 27;                                  // Differential AMP1
static const uint8_t AD2 = 28;                                  // Differential AMP2


#define GAIN5  0                                                // Gain values used as 2nd parameter in pinMode() w/AD0, AD1 or AD2
#define GAIN10 1
#define GAIN20 2
#define GAIN40 3

#define DAC_PORT    10                                          // ATmegaxxM1 has DAC on Port 10 (pin PC7).  Enable it with analogWrite()




// Following are used by SoftSerial lib to map ports to interupts.
// Port D10 does not support Soft Serial

#define digitalPinToPCICR(p)    (((p) == 10 || (p) < 0 || (p) > 23) ? ((uint8_t *)0) : (&PCICR))


#define digitalPinToPCICRbit(p) ( ((p) == 5  || (p) == 6) || \
                                  ((p) == 8  || (p) == 9) || \
                                  ((p) == 17)             || \
                                  ((p) >= 21 && (p) <=23) ? 0 : \
                                ( ((p) >=  2 && (p) <= 4) || \
                                  ((p) == 12)             || \
                                  ((p) >= 18 && (p) <=20) ? 1 : \
                                2))


#define digitalPinToPCMSK(p)    ( ((p) == 5  || (p) == 6) || \
                                  ((p) == 8  || (p) == 9) || \
                                  ((p) == 17)             || \
                                  ((p) >= 21 && (p) <=23) ? (&PCMSK0) : \
                                ( ((p) >=  2 && (p) <= 4) || \
                                  ((p) == 12)             || \
                                  ((p) >= 18 && (p) <=20) ? (&PCMSK1) : \
                                (&PCMSK2)))



#define digitalPinToPCMSKbit(p) ( ((p) == 5 || (p) == 11 || (p) == 12) ? 0 : \
                                ( ((p) == 2 || (p) ==  6 || (p) == 13) ? 1 : \
                                ( ((p) == 3 || (p) == 14 || (p) == 17) ? 2 : \
                                ( ((p) == 1 || (p) ==  4 || (p) ==  8) ? 3 : \
                                ( ((p) == 0 || (p) ==  9 || (p) == 18) ? 4 : \
                                ( ((p) == 15|| (p) == 19 || (p) == 21) ? 5 : \
                                ( ((p) == 16 ||(p) == 20 || (p) == 22) ? 6 : \
                                7)))))))





        
#ifndef PCICR                                                   // Correct errors in arduino IDE's avr VERSION used, missing definition of PCICR for atmegaxxm1
   #define PCICR _SFR_MEM8(0x68)                                // Later upgraded to avr (e.g., arduino 1.5.0 beta) should have this already defined.
   #define PCIE0 0                                              // So we do not need to define them here.  But if prior version, we DO need to define them here!
   #define PCIE1 1
   #define PCIE2 2
   #endif


#ifndef POCR_RBH                                                // Newer way to define this is to use '_', not 'x'...
   #define POCR_RBH POCRxRBH
   #endif
#ifndef POCR_RBL
   #define POCR_RBL POCRxRBL
   #endif







#ifdef ARDUINO_MAIN



// On the Arduino board, digital pins are also used
// for the analog output (software PWM).  Analog input
// pins are a separate set.

// ATMEL ATMEGA32M1 / ATMEGA64M1
//
// D0                           PD4             RX
// D1                           PD3             TX / TIMER0A / SS
// D2                           PC1                  TIMER1B
// D3                           PC2             Tx-CAN
// D4                           PC3             Rx-CAN
// D5                           PB0             MISO
// D6                           PB1             MOSI
// D7                           PD7
//
// D8                           PB3             
// D9                           PB4     
// D10                          PC7     
// D11                          PD0             
// D12                          PC0     
// D13                          PD1     
// D14                          PD2     
// D15          A0              PD5

// D16          A1              PD6
// D17          A2              PB2
// D18          A3              PC4                     
// D19          A4              PC5             
// D20          A5              PC6             
// D21          A6              PB5             
// D22          A7              PB6             
// D23          A8              PB7             SCK
//
//
// A0           D15             PD5             ADC2
// A1           D16             PD6             ADC3
// A2           D17             PB2             ADC5
// A3           D18             PC4             ADC8 / AMP1-
// A4           D19             PC5             ADC9 / AMP1+    
// A5           D20             PC6             ADC10
// A6           D21             PB5             ADC6 / AMP2-
// A7           D22             PB6             ADC7
// A8           D23             PB7             ADC4
//
// A9           D24             --              Internal core temp sensor
// A10          D25             --              Internal Vcc/4
// AD0          D26             --              Differential AMP0
// AD1          D27             --              Differential AMP1
// AD2          D28             --              Differential AMP2





// these arrays map port names (e.g. port B) to the
// appropriate addresses for various functions (e.g. reading
// and writing)
const uint16_t PROGMEM port_to_mode_PGM[] = {
        NOT_A_PORT,
        NOT_A_PORT,
        (uint16_t) &DDRB,
        (uint16_t) &DDRC,
        (uint16_t) &DDRD,
};

const uint16_t PROGMEM port_to_output_PGM[] = {
        NOT_A_PORT,
        NOT_A_PORT,
        (uint16_t) &PORTB,
        (uint16_t) &PORTC,
        (uint16_t) &PORTD,      
};

const uint16_t PROGMEM port_to_input_PGM[] = {
        NOT_A_PORT,
        NOT_A_PORT,
        (uint16_t) &PINB,
        (uint16_t) &PINC,
        (uint16_t) &PIND,
};

const uint8_t PROGMEM digital_pin_to_port_PGM[] = {
        PD, // D0 - PD4 - RX 
        PD, // D1 - PD3 - TX / ss / TIMER0A
        PC, // D2 - PC1 - TIMER1B
        PC, // D3 - PC2 - TXCAN
        PC, // D4 - PC3 - RXCAN
        PB, // D5 - PB0 - MOSI
        PB, // D6 - PB1 - MISO
        PD, // D7 - PD7
        
        PB, // D8 - PB3
        PB, // D9 - PB4
        PC, // D10 - PC7 
        PD, // D11 - PD0 
        PC, // D12 - PC0 
        PD, // D13 - PD1 
        PD, // D14 - PD2 - TIMER1A
        PD, // D15 - PD5 - A0

        PD, // D16 - PD6 - A1
        PB, // D17 - PB2 - A2
        PC, // D18 - PC4 - A3
        PC, // D19 - PC5 - A4
        PC, // D20 - PC6 - A5
        PB, // D21 - PB5 - A6
        PB, // D22 - PB6 - A7 
        PB, // D23 - PB7 - A8 (SCK) 
        
            //  D24 - na - A9   (Internal core temp sensor)
            //  D25 - na - A10  (Internal Vcc/4)
};



const uint8_t PROGMEM digital_pin_to_bit_mask_PGM[] = {
        _BV(4), /* 0 */
        _BV(3),
        _BV(1),
        _BV(2),
        _BV(3),
        _BV(0),
        _BV(1),
        _BV(7),
        
        _BV(3), /* 8 */
        _BV(4),
        _BV(7),
        _BV(0),
        _BV(0),
        _BV(1),
        _BV(2), 
        _BV(5),
        
        _BV(6), /* 16 */
        _BV(2),
        _BV(4),
        _BV(5),
        _BV(6),
        _BV(5),
        _BV(6),
        _BV(7),
};


const uint8_t PROGMEM digital_pin_to_timer_PGM[] = {
        NOT_ON_TIMER,
        TIMER0A,
        TIMER1B,
        NOT_ON_TIMER,
        NOT_ON_TIMER,   
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,
  
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,   
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        TIMER1A,
        NOT_ON_TIMER,

        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,   
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        NOT_ON_TIMER,
        

};


const uint8_t PROGMEM analog_pin_to_channel_PGM[14] = {
        2,      // A0           D15             PD5             ADC2
        3,      // A1           D16             PD6             ADC3
        5,      // A2           D17             PB2             ADC5
        8,      // A3           D18             PC4             ADC8 / AMP1-
        9,      // A4           D19             PC5             ADC9 / AMP1+    
        10,     // A5           D20             PC6             ADC10
        6,      // A6           D21             PB5             ADC6 / AMP2-
        7,      // A7           D22             PB6             ADC7
        4,      // A8           D23             PB7             ADC4

        11,     // A9           D24             --              Internal core temp sensor
        12,     // A10          D25             --              Internal Vcc/4
        14,     // AD0          D26             --              Differential AMP0
        15,     // AD1          D27             --              Differential AMP1
        16      // AD2          D28             --              Differential AMP2
};



#endif

#endif
