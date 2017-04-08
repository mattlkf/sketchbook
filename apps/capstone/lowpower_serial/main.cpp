#include <Arduino.h>
#include <LowPower.h>

#define LOW_POWER

#define BLINK_HIGH_MICROSECONDS 1

#define LEDPIN 13
#define MAX_BLINKS 1

void setup(){
	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, LOW);

	Serial.begin(115200);
}

int main(){
	init();
	setup();

	uint8_t i = 1;
	while(1){

#ifdef LOW_POWER
    // Enter power down state for 8 s with ADC and BOD module disabled
    LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);  
    // LowPower.powerDown(SLEEP_1S, ADC_OFF, BOD_OFF);  
#else
		delay(4000);
#endif
		
		Serial.println("Hi");
		Serial.flush();
	}

	return 0;
}