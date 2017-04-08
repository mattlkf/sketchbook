#include <Arduino.h>
#include <LowPower.h>

#define LOW_POWER

#define BLINK_HIGH_MICROSECONDS 1

#define LEDPIN 13
#define MAX_BLINKS 1

void setup(){
	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, LOW);
}

void blink(uint8_t k){
	for(uint8_t i = 0; i < k;i++){
		digitalWrite(LEDPIN, HIGH);
		delayMicroseconds(BLINK_HIGH_MICROSECONDS);
		
		digitalWrite(LEDPIN, LOW);
		if(i < k) delay(99);
	}
	return;
}

int main(){
	init();
	setup();

	uint8_t i = 1;
	while(1){

#ifdef LOW_POWER
    // Enter power down state for 8 s with ADC and BOD module disabled
    LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF);  
#else
		delay(4000);
#endif
		blink(i++);
		i = (i - 1) % MAX_BLINKS + 1;
	}

	return 0;
}