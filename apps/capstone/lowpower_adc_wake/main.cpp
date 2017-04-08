#include <Arduino.h>
#include <LowPower.h>

#define LOW_POWER

#define BLINK_HIGH_MICROSECONDS 1

// The pin that supplies voltage to the analog sensor
#define SENSOR_VCC 10 
#define SENSOR_INPUT A0

void setup(){
	pinMode(SENSOR_VCC, OUTPUT);
	digitalWrite(SENSOR_VCC, LOW);

	Serial.begin(115200);
}

int main(){
	init();
	setup();

	while(1){
    LowPower.powerDown(SLEEP_15MS, ADC_OFF, BOD_OFF);  
		
		digitalWrite(SENSOR_VCC, HIGH);
		// delay(10);
		uint16_t val;
		while((val = analogRead(SENSOR_INPUT)) > 512);
		// Serial.println(val);
		// Serial.flush();
		digitalWrite(SENSOR_VCC, LOW);
	}

	return 0;
}