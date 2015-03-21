#include <Arduino.h>

int main(){
    init();
    int val = 50;
    int pin = 11;
    Serial.println(val);
    pinMode(pin, OUTPUT);    
    analogWrite(pin, val);
    while(1);
    Serial.println("Bye");
    return 0;
}