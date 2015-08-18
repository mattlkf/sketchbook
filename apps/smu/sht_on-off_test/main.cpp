#include <Arduino.h>
#include <Wire.h>
#include <SHT2x.h> 

// This code tests whether it's OK to supply power to the SHT21 only when we need to make readings

void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");

    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);

    Wire.begin();
}

int main(){	
	init();
	setup();

	while(1){

        digitalWrite(5, HIGH);
        Serial.print("Humidity(%RH): ");
        Serial.println(SHT2x.GetHumidity());
        Serial.print("Temperature(C): ");
        Serial.println(SHT2x.GetTemperature());
        digitalWrite(5, LOW);
        delay(1000);
	}

    return 0;
}