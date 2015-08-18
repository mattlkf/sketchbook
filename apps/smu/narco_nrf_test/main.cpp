#include <Arduino.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

#include <Narcoleptic.h>

#define LEDPIN 13

#define NRF_PIN 10

// MIRF stuff
#define PAYLOADLEN 32 //max length
#define RXNAME "serv1" // 5 bytes
#define TXNAME "serv1" // 5 bytes

void setupMirf(){
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
  	Mirf.setRADDR((byte *) RXNAME);
    Mirf.setTADDR((byte *) TXNAME);
  	Mirf.payload = PAYLOADLEN;
  	Mirf.config();
}

// Main section

void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");

	pinMode(LEDPIN, OUTPUT);
	digitalWrite(LEDPIN, LOW);

	// pinMode(NRF_PIN, OUTPUT);
	// digitalWrite(NRF_PIN, HIGH);

	// delay(100);

	setupMirf();
}

void blink(uint8_t k){
	for(uint8_t i = 0; i < k;i++){
		digitalWrite(LEDPIN, HIGH);
		delayMicroseconds(1000);
		
		digitalWrite(LEDPIN, LOW);
		if(i < k) delay(99);
	}
}


int main(){
	int i;
	
	init();
	setup();

	uint32_t pkt_num = 0;
	while(1){

	// Blinkenlights
		// blink(2);

		Serial.println("Hello");

	// Creating a data buffer
		uint8_t data[PAYLOADLEN];

	// Sending data
		sprintf((char*)data, "Hello :) %d", pkt_num++);
		Mirf.send(data);

		while(Mirf.isSending()){
			Serial.println("wait..");
		}
		Serial.println("Sent");

	// Checking for available data
		if(!Mirf.isSending() && Mirf.dataReady()){

		// Get the data
			Mirf.getData(data);

		// Print the data (stopping at null terminator if any)
			for(i=0; data[i] && i<PAYLOADLEN; i++){
				Serial.write(data[i]);
			}
			Serial.println();
		}

	// Flush serial
		Serial.flush();

	// Turn off the radio (it auto-turns on later when you try to send)
		Mirf.powerDown();

	// Sleep
		Narcoleptic.delay(5000);

	}
}