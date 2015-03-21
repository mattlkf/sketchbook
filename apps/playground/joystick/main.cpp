#include <Arduino.h>

const uint8_t outPin = 11;
const uint8_t joyPin = A1;

void setup(){
    pinMode(outPin, OUTPUT);
    Serial.begin(9600);
    Serial.println("Ready");
}

void loop(){
    uint16_t val = analogRead(joyPin);
    Serial.print(val >> 2);
    Serial.print(" ");
    Serial.println(val);
    analogWrite(outPin, val >> 2);
    delay(50);
}

int main(){
    init();
    setup();
 
    while(1){
       loop();
    }
    return 0;
}