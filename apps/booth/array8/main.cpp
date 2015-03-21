#include <Arduino.h>

int main(){
    init();

    Serial.begin(9600);

    Serial.println("Ready");

    while(1){
        Serial.print(analogRead(A0));
        Serial.print("\t");
        Serial.print(analogRead(A1));
        Serial.print("\t");
        Serial.print(analogRead(A2));
        Serial.print("\t");
        Serial.print(analogRead(A3));
        Serial.print("\t");
        Serial.print(analogRead(A4));
        Serial.print("\t");
        Serial.print(analogRead(A5));
        Serial.print("\t");
        Serial.print(analogRead(A6));
        Serial.print("\t");
        Serial.print(analogRead(A7));
        Serial.println();
        delay(500);
    }
    return 0;
}
