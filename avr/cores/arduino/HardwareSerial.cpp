/*
  HardwareSerial.cpp - Hardware serial library for Wiring
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
  Modified 10 January 2013 by CODINGHEAD (Stuart Cording) for support of LIN UART (e.g. ATmega32M1 / ATmega64M1)
  Modified 20 March 2015 by Al Thomason (http://smartmppt.blogspot.com) to correct random hangs and other xxM1 bugs.
						(Still could use much cleaning up - but seems to work for now)

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"
#include "wiring_private.h"

// this next line disables the entire HardwareSerial.cpp, 
// this is so I can support Attiny series and any other chip without a uart
#if defined(UBRRH) || defined(UBRR0H) || defined(UBRR1H) || defined(UBRR2H) || defined(UBRR3H) || defined(LINBRRH)

#include "HardwareSerial.h"

/*
 * on ATmega8, the uart and its bits are not numbered, so there is no "TXC0"
 * definition.
 */
#if !defined(TXC0)
#if defined(TXC)
#define TXC0 TXC
#elif defined(LTXOK)
// There is no "TXC0" in LIN module. To make definitions easier
// we define that this is a "LIN_UART"
#define LIN_UART
#elif defined(TXC1)
// Some devices have uart1 but no uart0
#define TXC0 TXC1
#else
#error "TXC0 (or LTXOK) not definable in HardwareSerial.h"
#endif
#endif

// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which head is the index of the location
// to which to write the next incoming character and tail is the index of the
// location from which to read.
#if (RAMEND < 1000)
  #define SERIAL_BUFFER_SIZE 16
#else
  #define SERIAL_BUFFER_SIZE 64
#endif

struct ring_buffer
{
  unsigned char buffer[SERIAL_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
};

#if defined(USBCON)
  ring_buffer rx_buffer = { { 0 }, 0, 0};
  ring_buffer tx_buffer = { { 0 }, 0, 0};
#endif
#if defined(UBRRH) || defined(UBRR0H) || defined(LIN_UART)
  ring_buffer rx_buffer  =  { { 0 }, 0, 0 };
  ring_buffer tx_buffer  =  { { 0 }, 0, 0 };
#endif
#if defined(UBRR1H)
  ring_buffer rx_buffer1  =  { { 0 }, 0, 0 };
  ring_buffer tx_buffer1  =  { { 0 }, 0, 0 };
#endif
#if defined(UBRR2H)
  ring_buffer rx_buffer2  =  { { 0 }, 0, 0 };
  ring_buffer tx_buffer2  =  { { 0 }, 0, 0 };
#endif
#if defined(UBRR3H)
  ring_buffer rx_buffer3  =  { { 0 }, 0, 0 };
  ring_buffer tx_buffer3  =  { { 0 }, 0, 0 };
#endif

inline void store_char(unsigned char c, ring_buffer *buffer)
{
  int i = (unsigned int)(buffer->head + 1) % SERIAL_BUFFER_SIZE;

  // if we should be storing the received character into the location
  // just before the tail (meaning that the head would advance to the
  // current location of the tail), we're about to overflow the buffer
  // and so we don't write the character or advance the head.
  if (i != buffer->tail) {
    buffer->buffer[buffer->head] = c;
    buffer->head = i;
  }
}

#if !defined(USART0_RX_vect) && defined(USART1_RX_vect)
// do nothing - on the 32u4 the first USART is USART1
#else
#if !defined(USART_RX_vect) && !defined(SIG_USART0_RECV) && \
    !defined(SIG_UART0_RECV) && !defined(USART0_RX_vect) && \
	!defined(SIG_UART_RECV) && !defined(LIN_TC_vect)
  #error "Don't know what the Data Received vector is called for the first UART"
  #error "or the General LIN vector for the LIN UART"
#else
  void serialEvent() __attribute__((weak));
  void serialEvent() {}
  #define serialEvent_implemented
#if defined(USART_RX_vect)
  SIGNAL(USART_RX_vect)
#elif defined(SIG_USART0_RECV)
  SIGNAL(SIG_USART0_RECV)
#elif defined(SIG_UART0_RECV)
  SIGNAL(SIG_UART0_RECV)
#elif defined(USART0_RX_vect)
  SIGNAL(USART0_RX_vect)
#elif defined(SIG_UART_RECV)
  SIGNAL(SIG_UART_RECV)
#elif defined(LIN_TC_vect)
// NOTE: There are no seperate RX/TX interrupts, just a single
// "something was sent/was received" interrupt.
  SIGNAL(LIN_TC_vect)
#endif
  {
  #if defined(UDR0)
    if (bit_is_clear(UCSR0A, UPE0)) {
      unsigned char c = UDR0;
      store_char(c, &rx_buffer);
    } else {
      unsigned char c = UDR0;
    };
  #elif defined(UDR)
    if (bit_is_clear(UCSRA, PE)) {
      unsigned char c = UDR;
      store_char(c, &rx_buffer);
    } else {
      unsigned char c = UDR;
    };
  #elif defined(LIN_UART)
	// NOTE: Rx AND Tx interrupts are handled together here for the LIN Module in UART mode.
	
    if (bit_is_set(LINSIR, LRXOK)) {							// Did we arrive here because there is something received?
	  unsigned char c = LINDAT;							// Yes.  (Reading LINDAT clears the LRXOK bit)
	  store_char(c, &rx_buffer);
	}

	
	// Now, lets check to see if we arrived here becuase of a TX interupt (meaning we just completed sending a character)
	if (bit_is_set(LINSIR, LTXOK)) {
		if (tx_buffer.head == tx_buffer.tail) {				// Yes, are we all done? (Transmission buffer empty?)
		  cbi(LINENIR, LENTXOK);							// Buffer empty, so disable interrupts
		  sbi(LINSIR, LTXOK);								// And clear the interupt = TX flag bit.
        }
        else {
		unsigned char c = tx_buffer.buffer[tx_buffer.tail];		// There is more data in the output buffer. 
		tx_buffer.tail = (tx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
         	LINDAT = c;									// Send the next byte.  (This too will clear the LTXOK bit)
	   }
	}
  #else
    #error "UDR or LINDAT not defined"
  #endif
}
#endif
#endif

#if defined(USART1_RX_vect)
  void serialEvent1() __attribute__((weak));
  void serialEvent1() {}
  #define serialEvent1_implemented
  SIGNAL(USART1_RX_vect)
  {
    if (bit_is_clear(UCSR1A, UPE1)) {
      unsigned char c = UDR1;
      store_char(c, &rx_buffer1);
    } else {
      unsigned char c = UDR1;
    };
  }
#elif defined(SIG_USART1_RECV)
  #error SIG_USART1_RECV
#endif

#if defined(USART2_RX_vect) && defined(UDR2)
  void serialEvent2() __attribute__((weak));
  void serialEvent2() {}
  #define serialEvent2_implemented
  SIGNAL(USART2_RX_vect)
  {
    if (bit_is_clear(UCSR2A, UPE2)) {
      unsigned char c = UDR2;
      store_char(c, &rx_buffer2);
    } else {
      unsigned char c = UDR2;
    };
  }
#elif defined(SIG_USART2_RECV)
  #error SIG_USART2_RECV
#endif

#if defined(USART3_RX_vect) && defined(UDR3)
  void serialEvent3() __attribute__((weak));
  void serialEvent3() {}
  #define serialEvent3_implemented
  SIGNAL(USART3_RX_vect)
  {
    if (bit_is_clear(UCSR3A, UPE3)) {
      unsigned char c = UDR3;
      store_char(c, &rx_buffer3);
    } else {
      unsigned char c = UDR3;
    };
  }
#elif defined(SIG_USART3_RECV)
  #error SIG_USART3_RECV
#endif

void serialEventRun(void)
{
#ifdef serialEvent_implemented
  if (Serial.available()) serialEvent();
#endif
#ifdef serialEvent1_implemented
  if (Serial1.available()) serialEvent1();
#endif
#ifdef serialEvent2_implemented
  if (Serial2.available()) serialEvent2();
#endif
#ifdef serialEvent3_implemented
  if (Serial3.available()) serialEvent3();
#endif
}

// NOTE: No support for LIN based UART here on Transmit side
// as it is integrated above in the "Receive" interrupt as both
// Tx and Rx are handled by one vector on this module.
#if !defined(LIN_UART)
#if !defined(USART0_UDRE_vect) && defined(USART1_UDRE_vect)
// do nothing - on the 32u4 the first USART is USART1
#else
#if !defined(UART0_UDRE_vect) && !defined(UART_UDRE_vect) && !defined(USART0_UDRE_vect) && !defined(USART_UDRE_vect)
  #error "Don't know what the Data Register Empty vector is called for the first UART"
#elif defined(UART0_UDRE_vect)
ISR(UART0_UDRE_vect)
#elif defined(UART_UDRE_vect)
ISR(UART_UDRE_vect)
#elif defined(USART0_UDRE_vect)
ISR(USART0_UDRE_vect)
#elif defined(USART_UDRE_vect)
ISR(USART_UDRE_vect)
#endif
{
  if (tx_buffer.head == tx_buffer.tail) {
	// Buffer empty, so disable interrupts
#if defined(UCSR0B)
    cbi(UCSR0B, UDRIE0);
#else
    cbi(UCSRB, UDRIE);
#endif
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = tx_buffer.buffer[tx_buffer.tail];
    tx_buffer.tail = (tx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
	
  #if defined(UDR0)
    UDR0 = c;
  #elif defined(UDR)
    UDR = c;
  #else
    #error UDR not defined
  #endif
  }
}
#endif
#endif

#ifdef USART1_UDRE_vect
ISR(USART1_UDRE_vect)
{
  if (tx_buffer1.head == tx_buffer1.tail) {
	// Buffer empty, so disable interrupts
    cbi(UCSR1B, UDRIE1);
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = tx_buffer1.buffer[tx_buffer1.tail];
    tx_buffer1.tail = (tx_buffer1.tail + 1) % SERIAL_BUFFER_SIZE;
	
    UDR1 = c;
  }
}
#endif

#ifdef USART2_UDRE_vect
ISR(USART2_UDRE_vect)
{
  if (tx_buffer2.head == tx_buffer2.tail) {
	// Buffer empty, so disable interrupts
    cbi(UCSR2B, UDRIE2);
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = tx_buffer2.buffer[tx_buffer2.tail];
    tx_buffer2.tail = (tx_buffer2.tail + 1) % SERIAL_BUFFER_SIZE;
	
    UDR2 = c;
  }
}
#endif

#ifdef USART3_UDRE_vect
ISR(USART3_UDRE_vect)
{
  if (tx_buffer3.head == tx_buffer3.tail) {
	// Buffer empty, so disable interrupts
    cbi(UCSR3B, UDRIE3);
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = tx_buffer3.buffer[tx_buffer3.tail];
    tx_buffer3.tail = (tx_buffer3.tail + 1) % SERIAL_BUFFER_SIZE;
	
    UDR3 = c;
  }
}
#endif


// Constructors ////////////////////////////////////////////////////////////////
#if defined(LIN_UART)
HardwareSerial::HardwareSerial(ring_buffer *rx_buffer, ring_buffer *tx_buffer,
  volatile uint8_t *linbrrh, volatile uint8_t *linbrrl,
  volatile uint8_t *linsir, volatile uint8_t *linenir,
  volatile uint8_t *lincr, volatile uint8_t *lindat,
  volatile uint8_t *linbtr,
  uint8_t lcmd1, uint8_t lcmd0, uint8_t lrxok, uint8_t ltxok, uint8_t lentxok)
#else
HardwareSerial::HardwareSerial(ring_buffer *rx_buffer, ring_buffer *tx_buffer,
  volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
  volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
  volatile uint8_t *ucsrc, volatile uint8_t *udr,
  uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udrie, uint8_t u2x)
#endif
{
#if defined(LIN_UART)
  _rx_buffer = rx_buffer;
  _tx_buffer = tx_buffer;
  _linbrrh   = linbrrh;
  _linbrrl   = linbrrl;
  _linsir    = linsir;
  _linenir   = linenir;
  _lincr     = lincr;
  _lindat    = lindat;
  _linbtr    = linbtr;
  _lcmd1     = lcmd1;
  _lcmd0     = lcmd0;
  _lrxok     = lrxok;
  _ltxok     = ltxok;
  _lentxok   = lentxok;
#else
  _rx_buffer = rx_buffer;
  _tx_buffer = tx_buffer;
  _ubrrh = ubrrh;
  _ubrrl = ubrrl;
  _ucsra = ucsra;
  _ucsrb = ucsrb;
  _ucsrc = ucsrc;
  _udr = udr;
  _rxen = rxen;
  _txen = txen;
  _rxcie = rxcie;
  _udrie = udrie;
  _u2x = u2x;
#endif
}

// Public Methods //////////////////////////////////////////////////////////////

void HardwareSerial::begin(unsigned long baud)
{
  uint16_t baud_setting;
#if !defined(LIN_UART)
  bool use_u2x = true;
#endif

// This is irrelevant for the LIN UART
#if F_CPU == 16000000UL && !defined(LIN_UART)
  // hardcoded exception for compatibility with the bootloader shipped
  // with the Duemilanove and previous boards and the firmware on the 8U2
  // on the Uno and Mega 2560.
  if (baud == 57600) {
    use_u2x = false;
  }
#endif
#ifdef LIN_UART
  // Disable re-sync; 8-time bit sampling mode
  *_linbtr = 0x88;
  // Use LINBTR LBTR[5:0] of 8 to calcualte baud rate
  // NOTE: Baud rate is supposed to be very accurate
  // so no need for complex checking like with normal
  // mega USART
  baud_setting = (F_CPU / (8 * baud)) - 1;
  *_linbrrh = (uint8_t)(baud_setting >> 8);
  *_linbrrl = (uint8_t)(baud_setting & 0x00FF);
#else
try_again:
  
  if (use_u2x) {
    *_ucsra = 1 << _u2x;
    baud_setting = (F_CPU / 4 / baud - 1) / 2;
  } else {
    *_ucsra = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }
  
  if ((baud_setting > 4095) && use_u2x)
  {
    use_u2x = false;
    goto try_again;
  }

  // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
  *_ubrrh = baud_setting >> 8;
  *_ubrrl = baud_setting;
#endif

  transmitting = false;

#ifdef LIN_UART

  *_lincr = 0x00;							// Clear config register (also selects 8-bit, none, 1 stop bit)
  sbi(*_lincr, _lcmd0);						// Activate Tx side
  sbi(*_lincr, _lcmd1);						// Activate Rx side
  sbi(*_lincr, LCMD2);						// Activate UART mode (instead of LIN mode)
  sbi(*_lincr, LENA);							// Turn on UART
  *_linsir = 0x0F;							// Clear status and interrupt register bits
  
  sbi(*_linenir, LENRXOK);						// Enable rx interrupt
  cbi(*_linenir, _lentxok);					// Disable tx interrupts, LENTXOK is used as a  'flag' to indicate if the Tx buffer is idle or not.
#else
  sbi(*_ucsrb, _rxen);
  sbi(*_ucsrb, _txen);
  sbi(*_ucsrb, _rxcie);
  cbi(*_ucsrb, _udrie);
#endif
}

void HardwareSerial::begin(unsigned long baud, byte config)
{
  uint16_t baud_setting;
  uint8_t current_config;
#if !defined(LIN_UART)
  bool use_u2x = true;
#endif

#if F_CPU == 16000000UL && !defined(LIN_UART)
  // hardcoded exception for compatibility with the bootloader shipped
  // with the Duemilanove and previous boards and the firmware on the 8U2
  // on the Uno and Mega 2560.
  if (baud == 57600) {
    use_u2x = false;
  }
#endif

#ifdef LIN_UART
  // Disable re-sync; 8-time bit sampling mode
  *_linbtr = 0x88;
  // Use LINBTR LBTR[5:0] of 8 to calcualte baud rate
  // NOTE: Baud rate is supposed to be very accurate
  // so no need for complex checking like with normal
  // mega USART
  baud_setting = (F_CPU / (8 * baud)) - 1;
  *_linbrrh = (uint8_t)(baud_setting >> 8);
  *_linbrrl = (uint8_t)(baud_setting & 0x00FF);
#else
try_again:
  
  if (use_u2x) {
    *_ucsra = 1 << _u2x;
    baud_setting = (F_CPU / 4 / baud - 1) / 2;
  } else {
    *_ucsra = 0;
    baud_setting = (F_CPU / 8 / baud - 1) / 2;
  }
  
  if ((baud_setting > 4095) && use_u2x)
  {
    use_u2x = false;
    goto try_again;
  }

  // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
  *_ubrrh = baud_setting >> 8;
  *_ubrrl = baud_setting;
#endif

  //set the data bits, parity, and stop bits
#if defined(__AVR_ATmega8__)
  config |= 0x80; // select UCSRC register (shared with UBRRH)
#endif

#ifdef LIN_UART
  *_lincr = 0x00;							// Clear config register (also selects 8-bit, none, 1 stop bit)
  sbi(*_lincr, _lcmd0);						// Activate Tx side
  sbi(*_lincr, _lcmd1);						// Activate Rx side
  sbi(*_lincr, LCMD2);						// Activate UART mode (instead of LIN mode)
  sbi(*_lincr, LENA);							// Turn on UART
  *_linsir = 0x0F;							// Clear status and interrupt register bits
  
  sbi(*_linenir, LENRXOK);						// Enable rx interrupt
  cbi(*_linenir, _lentxok);					// Disable tx interrupts, LENTXOK is used as a  'flag' to indicate if the Tx buffer is idle or not.
#else
  *_ucsrc = config;

  sbi(*_ucsrb, _rxen);
  sbi(*_ucsrb, _txen);
  sbi(*_ucsrb, _rxcie);
  cbi(*_ucsrb, _udrie);
#endif
}

void HardwareSerial::end()
{
  // wait for transmission of outgoing data
  while (_tx_buffer->head != _tx_buffer->tail)
    ;

#ifdef LIN_UART
  // Turn off Rx and Tx side of UART
  cbi(*_lincr, _lcmd0);
  cbi(*_lincr, _lcmd1);
  // Turn off Rx and Tx interrupts
  cbi(*_linenir, LENRXOK);
  cbi(*_linenir, _lentxok);
#else
  cbi(*_ucsrb, _rxen);
  cbi(*_ucsrb, _txen);
  cbi(*_ucsrb, _rxcie);  
  cbi(*_ucsrb, _udrie);
#endif

  // clear any received data
  _rx_buffer->head = _rx_buffer->tail;
}


int HardwareSerial::available(void)
{
  return (unsigned int)(SERIAL_BUFFER_SIZE + _tx_buffer->head - _tx_buffer->tail) % SERIAL_BUFFER_SIZE;
}

int HardwareSerial::peek(void)
{
  if (_rx_buffer->head == _rx_buffer->tail) {
    return -1;
  } else {
    return _rx_buffer->buffer[_rx_buffer->tail];
  }
}

int HardwareSerial::read(void)
{
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer->head == _rx_buffer->tail) {
    return -1;
  } else {
    unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
    _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE;
    return c;
  }
}

void HardwareSerial::flush()
{

#ifdef LIN_UART
  while (bit_is_set(LINENIR, LENTXOK));				// Am using LENTXOK as flag to show Tx is doing something...
#else
  // UDR is kept full while the buffer is not empty, so TXC triggers when EMPTY && SENT
  while (transmitting && ! (*_ucsra & _BV(TXC0)));
#endif
  transmitting = false;
}

size_t HardwareSerial::write(uint8_t c)
{
  int i = (_tx_buffer->head + 1) % SERIAL_BUFFER_SIZE;
	
  // If the output buffer is full, there's nothing for it other than to 
  // wait for the interrupt handler to empty it a bit
  // ???: return 0 here instead?
  while (i == _tx_buffer->tail)
    ;

#ifdef LIN_UART
  unsigned char tmp = SREG;						// Globaly disable interupts while we figure things out.
  cli();										// So that we do not get caught between steps via the Tx interups.

  // Check if we are transmitting yet or not...
  if bit_is_clear(LINENIR, LENTXOK) {					// Tx is setting idle, need to kick-start it.
	sbi(LINENIR, LENTXOK);						// Enable Tx completed interupt
     LINDAT = c;								// And start this byte sending.
											// Further transmits will be now interrupt triggered
  }
  else {
    // If we are, store this byte in the buffer to be sent when we're ready
    _tx_buffer->buffer[_tx_buffer->head] = c;
    _tx_buffer->head = i;
  }
  SREG = tmp;									// Restore interupt status now that we have passed the critical timing spot
  transmitting = true;

#else
  _tx_buffer->buffer[_tx_buffer->head] = c;
  _tx_buffer->head = i;
  
  sbi(*_ucsrb, _udrie);
  // clear the TXC bit -- "can be cleared by writing a one to its bit location"
  transmitting = true;
  sbi(*_ucsra, TXC0);
#endif
  return 1;
}

HardwareSerial::operator bool() {
	return true;
}

// Preinstantiate Objects //////////////////////////////////////////////////////

#if defined(UBRRH) && defined(UBRRL)
  HardwareSerial Serial(&rx_buffer, &tx_buffer, &UBRRH, &UBRRL, &UCSRA, &UCSRB, &UCSRC, &UDR, RXEN, TXEN, RXCIE, UDRIE, U2X);
#elif defined(UBRR0H) && defined(UBRR0L)
  HardwareSerial Serial(&rx_buffer, &tx_buffer, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0, RXEN0, TXEN0, RXCIE0, UDRIE0, U2X0);
#elif defined(LIN_UART)
  HardwareSerial Serial(&rx_buffer, &tx_buffer, &LINBRRH, &LINBRRL, &LINSIR, &LINENIR, &LINCR, &LINDAT, &LINBTR, LCMD1, LCMD0, LRXOK, LTXOK, LENTXOK);

#elif defined(USBCON)
  // do nothing - Serial object and buffers are initialized in CDC code
#else
  #error no serial port defined  (port 0)
#endif

#if defined(UBRR1H)
  HardwareSerial Serial1(&rx_buffer1, &tx_buffer1, &UBRR1H, &UBRR1L, &UCSR1A, &UCSR1B, &UCSR1C, &UDR1, RXEN1, TXEN1, RXCIE1, UDRIE1, U2X1);
#endif
#if defined(UBRR2H)
  HardwareSerial Serial2(&rx_buffer2, &tx_buffer2, &UBRR2H, &UBRR2L, &UCSR2A, &UCSR2B, &UCSR2C, &UDR2, RXEN2, TXEN2, RXCIE2, UDRIE2, U2X2);
#endif
#if defined(UBRR3H)
  HardwareSerial Serial3(&rx_buffer3, &tx_buffer3, &UBRR3H, &UBRR3L, &UCSR3A, &UCSR3B, &UCSR3C, &UDR3, RXEN3, TXEN3, RXCIE3, UDRIE3, U2X3);
#endif

#endif // whole file

