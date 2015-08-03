#include <Arduino.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

// MIRF stuff
#define PAYLOADLEN 32 //max length
#define RXNAME "serv1" // 5 bytes
#define TXNAME "serv1" // 5 bytes

uint8_t data[PAYLOADLEN]; // a data buffer

void setupMirf(){
	Mirf.spi = &MirfHardwareSpi;
	Mirf.init();
  	Mirf.setRADDR((byte *) RXNAME);
    Mirf.setTADDR((byte *) TXNAME);
  	Mirf.payload = PAYLOADLEN;
  	Mirf.config();
}

void setup(){

	setupMirf();
}

uint32_t seqNo = 0;

void send(){
	// Write data
	snprintf((char*)data, PAYLOADLEN, "%d time: %u", seqNo++, millis());

	// Send
	Mirf.send(data);

	// Wait for the nRF to finish sending
	uint16_t cycles = 0;
	while(Mirf.isSending()){
		++cycles;
	}
}


int main(){
	init();
	setup();

	// A simple, headless send.
	while(1){
		send();
	}
	return 0;
}