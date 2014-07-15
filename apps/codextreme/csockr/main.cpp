#include <Arduino.h>
#include <SoftwareSerial.h>
#include <CheeseSock.h>

CheeseSock csock(new SoftwareSerial(4,5));

void callback(const uint8_t * str, uint8_t len){
	Serial.println("Received!");
	Serial.write(str, len);
	Serial.write("\n");
	return;
}

uint32_t timer = 0;

void setup(){
	Serial.begin(115200);
	Serial.println("Ready");

	csock.registerFn(2, callback);

	csock.begin(9600, 0);

	timer = millis();
}

int main(){
	init();

	setup();

	while(1){
		csock.run();

		if(millis() - timer > 3000){
			timer = millis();
			csock.send(2, "The quality of mercy is not strained/it droppeth as the gentle rain from heaven");
			Serial.println("Sent");
		}
	}
	return 0;
}