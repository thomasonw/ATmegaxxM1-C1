/* ------------------------------------------
  Example sketch to read messages via CAN.
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

  Can0.setNumTXBoxes(0); // use all mailboxes for receiving
  for (int filter = 0; filter < 4; filter++) //Set up 4 of the boxes for extended
  {
    Can0.setRXFilter(filter, 0, 0, true);
  }
  while (Can0.setRXFilter(0, 0, false) > 0); // Set up the remaining MObs for standard messages.
}

void loop() {
  CAN_FRAME incoming;

  if (Can0.rx_avail()) {
    if (Can0.read(incoming))
      printFrame(incoming);
    else
      Serial.print(" Failed to read CAN message!");
  }
}

// Function to print CAN message
void printFrame(CAN_FRAME &frame) {
  Serial.print("t=");
  Serial.print((millis() / 1000.0));
  Serial.print("s   ID: 0x");
  Serial.print(frame.id, HEX);
  Serial.print("   Len: ");
  Serial.print(frame.length);
  Serial.print(" bytes   Data: ");
  for (int count = 0; count < frame.length; count++) {
    Serial.print("0x");
    Serial.print(frame.data.bytes[count], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}
