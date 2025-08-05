#include <Arduino.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

void ledStartupTest() {
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
  }
}
