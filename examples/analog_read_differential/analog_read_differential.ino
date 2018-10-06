/* ------------------------------------------
  Example sketch to read differential analog
  inputs with gain.
  Pins: D9 (AMP0+) and D8 (AMP0-) for AD0

  Note: Floating point is only used for showing
        the functionality. For professional use
        it is recommend to use integer values.

  Author: Christoph Jurczyk
  https://github.com/christophjurczyk/
  ------------------------------------------ */

#define DIFFERNTIAL_INPUT AD0
#define SELECTED_GAIN 5 // according to pinMode for conversion
#define VCC 5.0 // in V

void setup() {
  Serial.begin(115200);

  // initialization  of differential input
  // choose between GAIN5, GAIN10, GAIN20 and GAIN40
  pinMode(DIFFERNTIAL_INPUT, GAIN5);
}

void loop() {

  Serial.print("Differential Input Voltage: ");
  Serial.print(readAnalogDiff());
  Serial.println(" V");

  delay(500);
}

// Returns differential input in V
float readAnalogDiff(void) {
  analogReference(INTERNAL);
  return analogRead(DIFFERNTIAL_INPUT) * VCC / (SELECTED_GAIN * 512);
}

