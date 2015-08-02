#include <Arduino.h>
#include <CapacitiveSensor.h>

/*
 * Capacitive "buzzers"!
 */

#define TIMEOUT -1

#define NS 3

CapacitiveSensor   cs[NS] = {
		CapacitiveSensor(5,2), 
		CapacitiveSensor(5,3), 
		CapacitiveSensor(5,8), 
	};

int led[NS] = {10,11,12};
int i;
int buzzed = 0;

uint32_t lastBuzz = 0;

void setup()                    
{
   for(i=0;i<NS;i++){
   	pinMode(led[i], OUTPUT);
   	// cs[i].set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
   }
   Serial.begin(9600);
}

void loop()                    
{
	// if you have input, then reset and clear everything
	if(Serial.available() || (TIMEOUT != -1 && buzzed && millis() - lastBuzz > TIMEOUT)){
		// clear serial input buffer
		char ch;
		while(Serial.available()){
			ch = Serial.read();
		}

		// turn off all lights
		for(i=0;i<NS;i++){
			digitalWrite(led[i], LOW);
		}
		buzzed = 0;
		Serial.println("Cleared");
	}
    long reading[NS];
    for(i=0;i<NS;i++) reading[i] = cs[i].capacitiveSensor(30);

    // for(i=0;i<NS;i++){
    // 	Serial.print(reading[i]);
    // 	Serial.print("\t");
    // }
    // Serial.println();

    for(i=0;i<NS && !buzzed;i++){
    	if(reading[i] > 250){
    		digitalWrite(led[i], HIGH);
    		buzzed = 1;
    		Serial.print("Buzzed! -- ");
    		Serial.println(i+1);
    		lastBuzz = millis();
    	}
    	else{
    		digitalWrite(led[i], LOW);
    	}
    }

    // delay(10);                             // arbitrary delay to limit data to serial port 
}

int main(){
	init();

	setup();
	while(1){
		loop();
	}
	return 0;
}