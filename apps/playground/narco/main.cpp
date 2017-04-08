#include <Arduino.h>
#include <Narcoleptic.h>
/*

-- Narco undef --

Before POWER LED cut:	27.68mA
Then POWER LED cut:		23.67mA
Then disable following:	24.89mA
	Timer1,2 and ADC
	Serial, Wire, SPI
Reenable above:		 	~24
Redisable + Narco sleep: 21.71mA

*/

#define NARCO

#define BLINK_HIGH_MICROSECONDS 1

#define LEDPIN 13
#define MAX_BLINKS 1

void setup(){
	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, LOW);

	Narcoleptic.disableMillis();
	Narcoleptic.disableTimer1();
	Narcoleptic.disableTimer2();
	Narcoleptic.disableSerial();
	Narcoleptic.disableADC();
	Narcoleptic.disableWire();
	Narcoleptic.disableSPI();
}

void blink(uint8_t k){
	Narcoleptic.enableMillis();
	for(uint8_t i = 0; i < k;i++){
		digitalWrite(LEDPIN, HIGH);
		delayMicroseconds(BLINK_HIGH_MICROSECONDS);
		
		digitalWrite(LEDPIN, LOW);
		if(i < k) delay(99);
	}
	Narcoleptic.disableMillis();
	return;
}

int main(){
	init();
	setup();

	uint8_t i = 1;
	while(1){

#ifdef NARCO
		Narcoleptic.delay(3000);
#else
		delay(3000);
#endif
		blink(i++);
		i = (i - 1) % MAX_BLINKS + 1;
	}

	return 0;
}