#include <Arduino.h>

volatile uint8_t fired = 0;

void pinInterrupt(void){
    ++fired;
    detachInterrupt(0);
}

void setup(){
    Serial.begin(9600);
    attachInterrupt(0, pinInterrupt, FALLING);
}

int main(){
    init();
    setup();

    uint32_t lastTime = 0;
    while(1){
        if(millis() - lastTime > 1000){
            lastTime = millis();
            Serial.println(digitalRead(2) == HIGH ? "HIGH" : "LOW");
        }
        if(fired){
            Serial.print("Fired! ");
            Serial.println(fired); // to see how many times the ISR was called in between polls to fired
            fired = 0;
            attachInterrupt(0, pinInterrupt, FALLING);
        }
    }

    return 0;
}