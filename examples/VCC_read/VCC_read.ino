/* ------------------------------------------
  Example sketch to read VCC of ATmega32M1/64M1

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

  Serial.print("VCC: ");
  Serial.print(readVCC());
  Serial.println(" V");

  delay(500);
}

// Returns VCC in V
float readVCC(void) {
  analogReference(INTERNAL);
  // Conversion:
  // analogRead will return 1/4 of VCC
  // To convert bits to V:
  // Value * 4 (due to 1/4 VCC) * 2.56 V (ADC Ref) / 1023 (ADC res)
  return analogRead(A10) * 4.0 * 2.56 / 1023 ;
}

