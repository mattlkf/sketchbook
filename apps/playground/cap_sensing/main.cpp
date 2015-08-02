#include <Arduino.h>
#include <CapacitiveSensor.h>

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */

CapacitiveSensor   cs[4] = {CapacitiveSensor(5,2), 
							CapacitiveSensor(5,3), 
							CapacitiveSensor(5,8), 
							CapacitiveSensor(5,9)};

int led[4] = {10,11,12,13};
int i;

void setup()                    
{
   // cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
   for(i=0;i<4;i++) pinMode(led[i], OUTPUT);
   Serial.begin(9600);
}

void loop()                    
{
    long total[4];
    for(i=0;i<4;i++) total[i] = cs[i].capacitiveSensor(30);

    for(i=0;i<4;i++){
    	Serial.print(total[i]);
    	Serial.print("\t");
    }
    Serial.println();

    for(i=0;i<4;i++){
    	if(total[i] > 150){
    		digitalWrite(led[i], HIGH);
    	}
    	else{
    		digitalWrite(led[i], LOW);
    	}
    }

    delay(10);                             // arbitrary delay to limit data to serial port 
}

int main(){
	init();

	setup();
	while(1){
		loop();
	}
	return 0;
}