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


// Main section

void setup(){
	Serial.begin(115200);
	Serial.println("Ready!");

	setupMirf();
}

uint8_t pressed(){
	uint8_t pflag = 0;
	while(Serial.available()){
		pflag = 1;
		Serial.read();
	}
	return pflag;
}

void send(){
	// Write data
	snprintf((char*)data, PAYLOADLEN, "time: %u", millis());

	// Send
	Mirf.send(data);

	// Wait for the nRF to finish sending
	uint16_t cycles = 0;
	while(Mirf.isSending()){
		++cycles;
	}

	// Feedback
	Serial.print("Sent (");
	Serial.print(cycles);
	Serial.print(" cycles)");
	Serial.println();
}

uint8_t received(){
	return (uint8_t)(!Mirf.isSending() && Mirf.dataReady());
}

void showdata(){
// Get the data
	Mirf.getData(data);

// Print the data (stopping at null terminator if any)
	uint8_t i;
	for(i=0; data[i] && i<PAYLOADLEN; i++){
		Serial.write(data[i]);
	}
	Serial.println();
}

int main(){
	init();
	setup();

	while(1){
		if(pressed()) send();
		if(received()) showdata();
	}
	return 0;
}