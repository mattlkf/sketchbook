// A simple square-wave generator, 0.5Hz to 10KHz
// Crude in the sense that I ignore the finite time taken to compute
// That will throw off timing.

#include <Arduino.h>

#define KNOB_A A1
#define LEDPIN 13

void setup(){
    Serial.begin(115200);
    Serial.println("Ready");

    pinMode(LEDPIN, OUTPUT);
}

int main(){
    init();

    setup();

    while(1){
        uint32_t a = analogRead(KNOB_A);

        a = (a * 2000) / 1023;

        Serial.println(a);

        if(a > 0) digitalWrite(LEDPIN, HIGH);

        delayMicroseconds(a);

        digitalWrite(LEDPIN, LOW);
        if(a < 2000) delayMicroseconds(2000-a);
    }

    return 0;
}