#include <Arduino.h>

// ---------- Піни ----------
constexpr uint8_t LDR_PIN = 4;
constexpr uint8_t LED_PIN = 5;

// ---------- SMA ----------
constexpr size_t SMA_SIZE = 16;
uint16_t smaBuf[SMA_SIZE];
size_t smaIndex = 0;
uint32_t smaSum = 0;
bool smaFilled = false;

constexpr uint16_t THRESH_DARK  = 1200;
constexpr uint16_t THRESH_LIGHT = 1800;

bool ledOn = false;

uint16_t smaUpdate(uint16_t sample) {
  smaSum -= smaBuf[smaIndex];
  smaBuf[smaIndex] = sample;
  smaSum += sample;
  smaIndex = (smaIndex + 1) % SMA_SIZE;
  if (smaIndex == 0) smaFilled = true;

  size_t n = smaFilled ? SMA_SIZE : smaIndex;
  if (n == 0) return sample;
  return (uint16_t)(smaSum / n);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  analogReadResolution(12); 

  for (size_t i = 0; i < SMA_SIZE; i++) smaBuf[i] = 0;

  Serial.println("LDR + SMA + hysteresis");
}

void loop() {
  uint16_t raw = analogRead(LDR_PIN);
  uint16_t filtered = smaUpdate(raw);

  if (!ledOn && filtered < THRESH_DARK) {
    ledOn = true;
    digitalWrite(LED_PIN, HIGH);
  } else if (ledOn && filtered > THRESH_LIGHT) {
    ledOn = false;
    digitalWrite(LED_PIN, LOW);
  }

  static uint32_t lastLog = 0;
  if (millis() - lastLog >= 300) {
    lastLog = millis();
    Serial.printf("RAW=%4u  SMA=%4u  LED=%s\n",
                  raw, filtered, ledOn ? "ON" : "OFF");
  }

  delay(20);
}