#include <Arduino.h>

#define TCRTPIN 0

void setup(){
	Serial.begin(115200);
	Serial.println("Ready");
}

int main(){
	init();
	setup();
	while(1){
		Serial.println(analogRead(TCRTPIN));
		delay(50);
	}
	return 0;
}