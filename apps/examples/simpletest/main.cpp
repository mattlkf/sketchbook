#include <Arduino.h>

void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");
}

int main(){
	init();
	setup();

	pinMode(13, OUTPUT);
	
	while(1){
		digitalWrite(13, LOW);
		delay(100);
		digitalWrite(13, HIGH);
		delay(100);

		Serial.println("!");
	}
}
