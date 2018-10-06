/* ------------------------------------------
  Example sketch to send messages via CAN.
  Install avr_can library from Al Thomason:
  1. Go to https://github.com/thomasonw/avr_can
  2. Click on "Clone or download" and download
     as a ZIP file.
  3. Select in the Arduino IDE Sketch -> Include
     Library -> Add .ZIP library
  4. Select downloaded ZIP file in the dialog.

  Author: Christoph Jurczyk
  https://github.com/christophjurczyk/
  ------------------------------------------ */

#include <avr_can.h>

void setup() {
  Serial.begin(115200);

  // Initialize CAN0 interface and set baudrate
  // Available baudrates: CAN_BPS_5K, CAN_BPS_10K,
  // CAN_BPS_25K, CAN_BPS_50K, CAN_BPS_125K, CAN_BPS_250K,
  // CAN_BPS_500K, CAN_BPS_800K and CAN_BPS_1000K 
  if (Can0.begin(CAN_BPS_125K)) {
    Serial.println("CAN Init OK");
  }  else  {
    Serial.println("CAN Init FAILED!");
    while (true) {};
  }
}

void loop() {

    boolean msgSent = sendData();

    if (msgSent) {
      Serial.println("CAN message sent!");
    } else {
      Serial.println("Error while sending CAN message! CAN Buffer full!");
    }

 

  delay(1000);
}

// Function to send a CAN message
bool sendData()
{
  CAN_FRAME outgoing; // declaration of CAN message object
  outgoing.id = 0x123; // ID of transmitted message
  outgoing.extended = false; // Extended frame format (29-bit ID instead of 11-bit)
  outgoing.priority = 4; // Transmission priority of message (0-15 lower is higher priority)
  outgoing.length   = 8; // Length of data

  // Data:
  outgoing.data.byte[0] = 0x01;
  outgoing.data.byte[1] = 0x23;
  outgoing.data.byte[2] = 0x45;
  outgoing.data.byte[3] = 0x67;
  outgoing.data.byte[4] = 0x89;
  outgoing.data.byte[5] = 0xAB;
  outgoing.data.byte[6] = 0xCD;
  outgoing.data.byte[7] = 0xEF;

  return (Can0.sendFrame(outgoing));
  // Take note:  sendFrame() really only places the message into the outgoing queue,
  //             and will return TRUE if is succeeds.  This does NOT mean that the
  //             message was actually  successfully sent at a later time.
}
