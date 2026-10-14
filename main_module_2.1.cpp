#include <Arduino.h>

// ───────── Конфігурація: жодних «магічних чисел» у решті коду ─────────
struct Config {
  static constexpr uint8_t  LED_PIN            = 5;
  static constexpr bool     LED_ACTIVE_HIGH    = true;
  static constexpr uint8_t  BUTTON_PIN         = 15;
  static constexpr uint8_t  BUTTON_PRESSED     = LOW;     // кнопка на GND + INPUT_PULLUP
  static constexpr uint32_t BLINK_INTERVAL_MS  = 500;
  static constexpr uint32_t DEBOUNCE_MS        = 50;
  static constexpr uint32_t STATS_WINDOW       = 1000;    // скільки ітерацій у звіті
  static constexpr uint32_t SERIAL_BAUD        = 115200;
};
static_assert(Config::BLINK_INTERVAL_MS > 0, "Інтервал блимання має бути > 0");

enum class LedState : uint8_t { Off, On };
enum class LedMode  : uint8_t { Blink, AlwaysOn, AlwaysOff };

constexpr LedMode nextMode(LedMode m) {
  return m == LedMode::Blink    ? LedMode::AlwaysOn
       : m == LedMode::AlwaysOn ? LedMode::AlwaysOff
                                : LedMode::Blink;
}

// ───────── Драйвер світлодіода ─────────
class Led {
public:
  constexpr Led(uint8_t pin, bool activeHigh) : pin_(pin), activeHigh_(activeHigh) {}

  void init() {
    pinMode(pin_, OUTPUT);
    set(LedState::Off);
  }

  void set(LedState state) {
    state_ = state;
    const bool level = (state == LedState::On) == activeHigh_;
    digitalWrite(pin_, level ? HIGH : LOW);
  }

  LedState state() const { return state_; }

private:
  const uint8_t pin_;
  const bool    activeHigh_;
  LedState      state_ = LedState::Off;
};

// ───────── Статистика часу ітерації loop() ─────────
class LoopStats {
public:
  void add(uint32_t us) {
    sum_ += us;
    ++count_;
    if (us < min_) min_ = us;
    if (us > max_) max_ = us;
  }

  bool ready() const { return count_ >= Config::STATS_WINDOW; }

  void reportAndReset() {
    Serial.printf("loop: avg=%.2f us, min=%lu us, max=%lu us (%lu iterations)\n",
                  static_cast<double>(sum_) / count_,
                  static_cast<unsigned long>(min_),
                  static_cast<unsigned long>(max_),
                  static_cast<unsigned long>(count_));
    sum_ = 0; count_ = 0; min_ = UINT32_MAX; max_ = 0;
  }

private:
  uint64_t sum_   = 0;
  uint32_t count_ = 0;
  uint32_t min_   = UINT32_MAX;
  uint32_t max_   = 0;
};

// ───────── Єдина «глобальна» змінна для ISR ─────────
namespace {
volatile bool g_buttonEvent = false;

void IRAM_ATTR onButtonIsr() {
  g_buttonEvent = true;            // ISR мінімальний: лише прапорець
}
}  // namespace

// ───────── Застосунок (superloop) ─────────
class Application {
public:
  void begin() {
    Serial.begin(Config::SERIAL_BAUD);
    led_.init();
    pinMode(Config::BUTTON_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(Config::BUTTON_PIN), onButtonIsr, FALLING);
    applyMode(millis());
  }

  void update() {
    const uint32_t startUs = micros();
    const uint32_t nowMs   = millis();

    handleButton(nowMs);
    updateLed(nowMs);

    stats_.add(micros() - startUs);          // замір до друку, щоб друк не спотворював результат
    if (stats_.ready()) stats_.reportAndReset();
  }

private:
  bool takeButtonEvent() {
    noInterrupts();                          // атомарно прочитати і скинути прапорець
    const bool event = g_buttonEvent;
    g_buttonEvent = false;
    interrupts();
    return event;
  }

  void handleButton(uint32_t nowMs) {
    if (!takeButtonEvent()) return;
    if (nowMs - lastPressMs_ < Config::DEBOUNCE_MS) return;                 // антидребезг
    if (digitalRead(Config::BUTTON_PIN) != Config::BUTTON_PRESSED) return;  // це не натискання

    lastPressMs_ = nowMs;
    mode_ = nextMode(mode_);
    applyMode(nowMs);
  }

  void applyMode(uint32_t nowMs) {
    switch (mode_) {
      case LedMode::Blink:
        led_.set(LedState::On);
        lastToggleMs_ = nowMs;
        break;
      case LedMode::AlwaysOn:  led_.set(LedState::On);  break;
      case LedMode::AlwaysOff: led_.set(LedState::Off); break;
    }
  }

  void updateLed(uint32_t nowMs) {
    if (mode_ != LedMode::Blink) return;
    if (nowMs - lastToggleMs_ >= Config::BLINK_INTERVAL_MS) {
      lastToggleMs_ += Config::BLINK_INTERVAL_MS;                           // без накопичення похибки
      led_.set(led_.state() == LedState::On ? LedState::Off : LedState::On);
    }
  }

  Led      led_{Config::LED_PIN, Config::LED_ACTIVE_HIGH};
  LoopStats stats_;
  LedMode  mode_          = LedMode::Blink;
  uint32_t lastToggleMs_  = 0;
  uint32_t lastPressMs_   = 0;
};

static Application app;

void setup() { app.begin(); }
void loop()  { app.update(); }