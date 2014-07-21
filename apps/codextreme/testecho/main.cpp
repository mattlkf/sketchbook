#include <Arduino.h>

int main(){
	init();
	Serial.begin(57600);
	Serial.println("Ready");
	
	uint32_t prev = millis();
	while(1){
		if(Serial.available()){
			Serial.write((char)Serial.read());
		}

		if(millis() - prev > 500){
			prev = millis();
			Serial.println("Hello");
		}
	}
	return 0;
}