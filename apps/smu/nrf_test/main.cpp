#include <Arduino.h>
#include <SPI.h>
#include <Mirf.h>
#include <nRF24L01.h>
#include <MirfHardwareSpiDriver.h>

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

	setupMirf();
}

int main(){
	int i;
	
	init();
	setup();

	pinMode(13, OUTPUT);

	while(1){

	// Blinkenlights
		digitalWrite(13, HIGH);
		delay(100);
		digitalWrite(13, LOW);
		delay(900);

		Serial.println("Hello");

	// Creating a data buffer
		uint8_t data[PAYLOADLEN];

	// Sending data
		sprintf((char*)data, "Hello :)");
		Mirf.send(data);

		while(Mirf.isSending()){
			Serial.println("wait..");
		}
		Serial.println("Sent");

	// Checking for available data
		if(Mirf.isSending() || !Mirf.dataReady()) continue;

	// Get the data
		Mirf.getData(data);

	// Print the data (stopping at null terminator if any)
		for(i=0; data[i] && i<PAYLOADLEN; i++){
			Serial.write(data[i]);
		}
		Serial.println();
	}
}