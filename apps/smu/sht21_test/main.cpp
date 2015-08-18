#include <Arduino.h>
#include <Wire.h>
#include <SHT2x.h> 


void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");

    pinMode(5, OUTPUT);
    digitalWrite(5, HIGH);
    
    delay(100);

    Wire.begin();
}

int main(){	
	init();
	setup();

	while(1){
        Serial.print("Humidity(%RH): ");
        Serial.println(SHT2x.GetHumidity());
        Serial.print("Temperature(C): ");
        Serial.println(SHT2x.GetTemperature());

        delay(1000);
	}

    return 0;
}