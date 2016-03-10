/*
  HardwareSerial0.cpp - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 23 November 2006 by David A. Mellis
  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
  Modified 3 December 2013 by Matthijs Kooijman
  
    Modified to support ATmega32M1, ATmega64M1, etc.   Mar 2016 
    based on work from CODINGHEAD (Stuart Cording)
        Al Thomason:   https://github.com/thomasonw/ATmegaxxM1-C1
                       http://smartmppt.blogspot.com/search/label/xxM1-IDE
                       
                       
*/

#include "Arduino.h"
#include "HardwareSerial.h"
#include "HardwareSerial_private.h"

// Each HardwareSerial is defined in its own file, sine the linker pulls
// in the entire file when any element inside is used. --gc-sections can
// additionally cause unused symbols to be dropped, but ISRs have the
// "used" attribute so are never dropped and they keep the
// HardwareSerial instance in as well. Putting each instance in its own
// file prevents the linker from pulling in any unused instances in the
// first place.

#if defined(HAVE_HWSERIAL0)

#if defined(USART_RX_vect)
  ISR(USART_RX_vect)
#elif defined(USART0_RX_vect)
  ISR(USART0_RX_vect)
#elif defined(USART_RXC_vect)
  ISR(USART_RXC_vect) // ATmega8
#elif defined(LIN_TC_vect)
  ISR(LIN_TC_vect)      // ATmegaxxM1/C1
#else
  #error "Don't know what the Data Received vector is called for Serial"
#endif
  {
    Serial._rx_complete_irq();
  }


#if !defined(LINBRRH)
#if defined(UART0_UDRE_vect)
ISR(UART0_UDRE_vect)
#elif defined(UART_UDRE_vect)
ISR(UART_UDRE_vect)
#elif defined(USART0_UDRE_vect)
ISR(USART0_UDRE_vect)
#elif defined(USART_UDRE_vect)
ISR(USART_UDRE_vect)
#else
  #error "Don't know what the Data Register Empty vector is called for Serial"
#endif
{
  Serial._tx_udr_empty_irq();
}
#endif                          // !LIN_UART  -- Only one IRQ handler for the LIN UART, all is handled by the Rx one.


#if defined(UBRRH) && defined(UBRRL)
  HardwareSerial Serial(&UBRRH, &UBRRL, &UCSRA, &UCSRB, &UCSRC, &UDR);
#elif defined(LINBRRH)
  HardwareSerial Serial(&LINBRRH, &LINBRRL, &LINSIR, &LINENIR, &LINCR, &LINDAT);
#else
  HardwareSerial Serial(&UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0);
#endif

// Function that can be weakly referenced by serialEventRun to prevent
// pulling in this file if it's not otherwise used.
bool Serial0_available() {
  return Serial.available();
}

#endif // HAVE_HWSERIAL0
