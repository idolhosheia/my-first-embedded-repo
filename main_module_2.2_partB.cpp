#include <Arduino.h>

const int POT_PIN = 1;
const int MOTOR_PIN = 2;

const unsigned long PWM_PERIOD = 10000;

unsigned long previousMicros = 0;
unsigned long highTime = 0;
bool pinState = false;

unsigned long previousMillis = 0;
const unsigned long SERIAL_INTERVAL = 250;

void setup() {
  pinMode(MOTOR_PIN, OUTPUT);
  digitalWrite(MOTOR_PIN, LOW);
  
  Serial.begin(115200); 
}

void loop() {
  int adcValue = analogRead(POT_PIN);
  highTime = map(adcValue, 0, 4095, 0, PWM_PERIOD);
  unsigned long currentMicros = micros();
  unsigned long timePassed = currentMicros - previousMicros;

  if (timePassed >= PWM_PERIOD) {
    previousMicros = currentMicros;
    
    if (highTime > 0) {
      digitalWrite(MOTOR_PIN, HIGH);
      pinState = true;
    } else {
      digitalWrite(MOTOR_PIN, LOW);
      pinState = false;
    }
  } 
  else if (pinState && (timePassed >= highTime)) {
    digitalWrite(MOTOR_PIN, LOW);
    pinState = false;
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= SERIAL_INTERVAL) {
    previousMillis = currentMillis;
    
    Serial.print("ADC Value: ");
    Serial.println(adcValue);
  }
}


// ADC Value: 0
// ADC Value: 0
// ADC Value: 0
// ADC Value: 0
// ADC Value: 0
// ADC Value: 185
// ADC Value: 430
// ADC Value: 841
// ADC Value: 1271
// ADC Value: 1771
// ADC Value: 2137
// ADC Value: 2388
// ADC Value: 2781
// ADC Value: 3263
// ADC Value: 3791
// ADC Value: 4079
// ADC Value: 4095
// ADC Value: 4095
// ADC Value: 4095
// ADC Value: 4095
// ADC Value: 4095
// ADC Value: 4095
// ADC Value: 4029
// ADC Value: 3787
// ADC Value: 3387
// ADC Value: 3127
// ADC Value: 2923
// ADC Value: 2747
// ADC Value: 2597
// ADC Value: 2321
// ADC Value: 2276
// ADC Value: 2183
// ADC Value: 2195
// ADC Value: 2197
// ADC Value: 1899
// ADC Value: 1816
// ADC Value: 1629
// ADC Value: 1439
// ADC Value: 1215
// ADC Value: 1011
// ADC Value: 747
// ADC Value: 429
// ADC Value: 37
// ADC Value: 0
// ADC Value: 0
// ADC Value: 0
