#include <Arduino.h>
#include <Narcoleptic.h>

#define LEDPIN 13
#define MAX_BLINKS 1

void setup(){
	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, LOW);

	Serial.begin(9600);


	Narcoleptic.disableMillis();
	Narcoleptic.disableTimer1();
	Narcoleptic.disableTimer2();
	// Narcoleptic.disableSerial();
	Narcoleptic.disableADC();
	Narcoleptic.disableWire();
	Narcoleptic.disableSPI();

	Narcoleptic.useInterrupt(1, FALLING);
}

void blink(uint8_t k){
	Narcoleptic.enableMillis();
	for(uint8_t i = 0; i < k;i++){
		digitalWrite(LEDPIN, HIGH);
		delayMicroseconds(1000);
		
		digitalWrite(LEDPIN, LOW);
		if(i < k) delay(999);
	}
	Narcoleptic.disableMillis();
	return;
}

int main(){
	init();
	setup();

	uint8_t i = 1;
	while(1){
		Narcoleptic.delay(10000);
		blink(1);
		if(Narcoleptic.wasInterrupted()){
			// blink(2);
			// Narcoleptic.enableMillis();
			// Narcoleptic.enableSerial();
			Serial.println("Interrupt!");
			Serial.flush();
			// Narcoleptic.disableSerial();
			// delay(1000);
			// Narcoleptic.disableMillis();
			blink(3);
		}

	}

	return 0;
}