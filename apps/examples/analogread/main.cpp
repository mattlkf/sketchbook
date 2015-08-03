#include <Arduino.h>

#define PIN A5

int main(){
    init();

    Serial.begin(9600);

    Serial.println("Ready");

    while(1){
        Serial.println(analogRead(PIN));
        delay(100);
    }
    return 0;
}
