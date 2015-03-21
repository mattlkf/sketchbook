#include <Arduino.h>

#define KNOB A1
#define LASER 11

void setup(){
    Serial.begin(57600);
}

int main(){
    init();
    setup();
    while(1){
        uint16_t pwm = analogRead(KNOB) / 4;
        analogWrite(LASER, pwm);
        Serial.println(pwm);
    }

    return 0;

}