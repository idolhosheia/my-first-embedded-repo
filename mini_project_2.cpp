#include <Arduino.h>

// Піни (змініть під свою плату)
constexpr uint8_t PIN_RED    = 4;
constexpr uint8_t PIN_YELLOW = 5;
constexpr uint8_t PIN_GREEN  = 6;

// Тривалість фаз (мс) — для дебагу можна зменшити
constexpr uint32_t T_GREEN          = 5000;
constexpr uint32_t T_GREEN_BLINK    = 3000;
constexpr uint32_t T_YELLOW         = 2000;
constexpr uint32_t T_RED            = 5000;
constexpr uint32_t T_RED_YELLOW     = 2000;
constexpr uint32_t BLINK_PERIOD     = 300;  // період миготіння зеленого

enum class LightState : uint8_t {
  Green,
  GreenBlink,
  Yellow,
  Red,
  RedYellow
};

LightState state = LightState::Green;
uint32_t stateStart = 0;
uint32_t lastBlink = 0;
bool blinkOn = true;

void setLights(bool r, bool y, bool g) {
  digitalWrite(PIN_RED,    r ? HIGH : LOW);
  digitalWrite(PIN_YELLOW, y ? HIGH : LOW);
  digitalWrite(PIN_GREEN,  g ? HIGH : LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_YELLOW, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);

  stateStart = millis();
  setLights(false, false, true);
  Serial.println("Traffic light (UA PDR) started");
}

void loop() {
  uint32_t now = millis();
  uint32_t elapsed = now - stateStart;

  switch (state) {
    case LightState::Green:
      setLights(false, false, true);
      if (elapsed >= T_GREEN) {
        state = LightState::GreenBlink;
        stateStart = now;
        lastBlink = now;
        blinkOn = true;
        Serial.println("STATE: Green blinking");
      }
      break;

    case LightState::GreenBlink:
      if (now - lastBlink >= BLINK_PERIOD) {
        lastBlink = now;
        blinkOn = !blinkOn;
      }
      setLights(false, false, blinkOn);
      if (elapsed >= T_GREEN_BLINK) {
        state = LightState::Yellow;
        stateStart = now;
        setLights(false, true, false);
        Serial.println("STATE: Yellow");
      }
      break;

    case LightState::Yellow:
      setLights(false, true, false);
      if (elapsed >= T_YELLOW) {
        state = LightState::Red;
        stateStart = now;
        setLights(true, false, false);
        Serial.println("STATE: Red");
      }
      break;

    case LightState::Red:
      setLights(true, false, false);
      if (elapsed >= T_RED) {
        state = LightState::RedYellow;
        stateStart = now;
        setLights(true, true, false);
        Serial.println("STATE: Red + Yellow");
      }
      break;

    case LightState::RedYellow:
      setLights(true, true, false);
      if (elapsed >= T_RED_YELLOW) {
        state = LightState::Green;
        stateStart = now;
        setLights(false, false, true);
        Serial.println("STATE: Green");
      }
      break;
  }
}