#include <Arduino.h>

constexpr uint8_t  POT_PIN   = 4;
constexpr uint16_t ADC_MAX   = 4095;
constexpr float    U_REF_MV  = 3300.0f;
constexpr uint32_t PERIOD_MS = 100;

void setup() {
  Serial.begin(115200);
  delay(500);

  analogReadResolution(12);
  analogSetPinAttenuation(POT_PIN, ADC_11db);

  Serial.println();
  Serial.println("ADC calibration (potentiometer)");
  Serial.println("Ref=3300 mV | Resolution=12-bit | Atten=11 dB | Period=100 ms");
  Serial.println();
  Serial.println("RAW    U_manual(mV)   U_cali(mV)   Error(%)");
  Serial.println("----------------------------------------------");
}

void loop() {
  int raw = analogRead(POT_PIN); //raw

  float uManual = (raw / (float)ADC_MAX) * U_REF_MV; //manual

  int uCali = analogReadMilliVolts(POT_PIN); //calibrated

  float error = 0.0f; //error %
  if (uCali != 0) {
    error = fabsf(uManual - (float)uCali) / (float)uCali * 100.0f;
  }

  Serial.printf("%4d   %8.1f      %6d      %6.2f\n",
                raw, uManual, uCali, error);

  delay(PERIOD_MS);
}