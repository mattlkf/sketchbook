#include <Arduino.h>

#define OUT_PIN 13

int main(){
  init();

  pinMode(OUT_PIN, OUTPUT);

  uint8_t i = 0;
  while(1){
    digitalWrite(OUT_PIN, HIGH);
    // delay(50 + i / 16);
    // i += 16;

    delay(i+1);
    i = 1 - i;

    digitalWrite(OUT_PIN, LOW);
    delay(50);
  }

  return 0;
}