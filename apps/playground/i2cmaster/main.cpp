#include <Arduino.h>
#include <Wire.h>

void setup(){
    Serial.begin(9600);

    Wire.begin();
    Serial.println("Ready");

}

void loop(){
    Wire.requestFrom(2, 6);
    while(Wire.available()){
        char c = Wire.read();
        Serial.print(c);
    }
    delay(500);
}



int main(){
    init();
    setup();
    while(1){
        loop();
    }

    return 0;
}