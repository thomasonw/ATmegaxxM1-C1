/* ------------------------------------------
  Example sketch for writing a sine wave via
  the internal DAC.
  Sketch reads value of sine from lookup table
  (sine_table.h) and writes value to internal
  DAC.

  Author: Christoph Jurczyk
  https://github.com/christophjurczyk/
  ------------------------------------------ */

// Variables
uint16_t output_signal = 0;
uint16_t index = 0;
// Includes
#include "sine_table.h"

void setup() {
}

void loop() {
  // signal generation
  output_signal = sine_table[index];
  index++;
  if (index == 500) index = 0;

  // output to DAC
  analogWrite(DAC_PORT, output_signal);
}



