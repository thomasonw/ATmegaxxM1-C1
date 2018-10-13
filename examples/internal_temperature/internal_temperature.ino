/* ------------------------------------------
  Example sketch to read internal temperature
  sensor of ATmega32M1/64M1

  Note: Floating point is only used for showing
        the functionality. For professional use
        it is recommend to use integer values.

  Author: Christoph Jurczyk
  https://github.com/christophjurczyk/
  ------------------------------------------ */


void setup() {
  Serial.begin(115200);
}

void loop() {

  Serial.print("Internal Temperature: ");
  Serial.print(readInternalTemp());
  Serial.println(" degC");

  delay(500);
}

// Returns internal temperature in degC
float readInternalTemp(void) {
  analogReference(INTERNAL);
  // Conversion:
  // 2.5 mV/degC (datasheet 18.8) = 0.4 degC/mV
  // Gain: 0.4*2560/1023 = 1 degC/bit
  // Offset: 25 degC - Gain*(762 mV / 2560 mV)*1023 = -279.5
  // I needed to add to the theoretical value about 20 degC as correction
  return analogRead(A9) - 279.5 + 20.0;
}

