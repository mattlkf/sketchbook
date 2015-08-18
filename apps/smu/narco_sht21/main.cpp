#include <Arduino.h>
#include <Narcoleptic.h>

#include <Wire.h>
#include <SHT2x.h> 

#define LEDPIN 13
#define SHT_PIN 5

#define PRINT_TO_SERIAL

void setup(){
	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, LOW);

	pinMode(SHT_PIN, OUTPUT);
	digitalWrite(SHT_PIN, LOW);

	// digitalWrite(SHT_PIN, HIGH);
	Wire.begin();
	// digitalWrite(SHT_PIN, LOW);

	// Narcoleptic.disableMillis();
	Narcoleptic.disableTimer1();
	Narcoleptic.disableTimer2();
	#ifndef PRINT_TO_SERIAL 
		Narcoleptic.disableSerial(); // if we are not using serial at all, then disable it from the start
	#endif
	Narcoleptic.disableADC();
	// Narcoleptic.disableWire();
	Narcoleptic.disableSPI();

	Narcoleptic.useInterrupt(1, FALLING);
}

void blink(uint8_t k){
	for(uint8_t i = 0; i < k;i++){
		digitalWrite(LEDPIN, HIGH);
		delayMicroseconds(1000);
		
		digitalWrite(LEDPIN, LOW);
		if(i < k) delay(999);
	}
	return;
}

int main(){
	init();
	setup();

	uint8_t i = 1;
	while(1){
		#ifdef PRINT_TO_SERIAL
			// Turn off serial during sleep.. ?
			Serial.flush();
			Serial.end();
		#endif
		

		// Disable stuff needed during awake-time
		#ifdef PRINT_TO_SERIAL
			Narcoleptic.disableSerial();
		#endif
		Narcoleptic.disableMillis();
		Narcoleptic.disableWire();

		// this is necessary to bring current consumption down from 0.35mA to 0.15mA in sleep
		digitalWrite(A4, LOW); // SDA low
		digitalWrite(A5, LOW); // SCL low

		// Sleep
		Narcoleptic.delay(2000);
		// Re-enable stuff needed during awake-time
		Narcoleptic.enableWire(); // i2c
		Narcoleptic.enableMillis(); // without this, i2c doesn't work
		#ifdef PRINT_TO_SERIAL
			Narcoleptic.enableSerial(); // printing
		#endif

		// Wire.begin();

		#ifdef PRINT_TO_SERIAL
		// Re-start serial
		Serial.begin(115200);
		#endif


		if(Narcoleptic.wasInterrupted()){
			// Print something if we woke up due to an interrupt
			#ifdef PRINT_TO_SERIAL
				Serial.println("Interrupt!");
			#endif
			blink(3);
		}
		else{
			// Read and print if we woke up without interrupt

			// Take a set of readings 
			digitalWrite(SHT_PIN, HIGH);
			float h = SHT2x.GetHumidity();
			float t = SHT2x.GetTemperature();
			digitalWrite(SHT_PIN, LOW);
			
			#ifdef PRINT_TO_SERIAL
				// Print to serial
		        Serial.print("Humidity(%RH): ");
		        Serial.println(h);
		        Serial.print("Temperature(C): ");
		        Serial.println(t);
		    #endif
		}
	}

	return 0;
}