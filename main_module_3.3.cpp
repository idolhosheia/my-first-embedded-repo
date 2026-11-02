#include <Arduino.h>

constexpr uint8_t POT_PIN  = 4;
constexpr uint8_t LED_PIN  = 5;
constexpr uint8_t MOTOR_PIN = 6;


constexpr int LED_CH    = 0;
constexpr int MOTOR_CH  = 1;
constexpr int PWM_FREQ  = 5000;
constexpr int PWM_RES   = 12;

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);

  ledcSetup(LED_CH,   PWM_FREQ, PWM_RES);
  ledcSetup(MOTOR_CH, PWM_FREQ, PWM_RES);
  ledcAttachPin(LED_PIN,   LED_CH);
  ledcAttachPin(MOTOR_PIN, MOTOR_CH);

  Serial.println("Module 3.3: PWM — pot -> LED + motor");
}

void loop() {
  int raw = analogRead(POT_PIN);   // 0..4095

  ledcWrite(LED_CH,   raw);
  ledcWrite(MOTOR_CH, raw);

  static uint32_t t = 0;
  if (millis() - t >= 200) {
    t = millis();
    float duty = raw * 100.0f / 4095.0f;
    Serial.printf("RAW=%4d  duty=%.1f%%\n", raw, duty);
  }

  delay(10);
}
