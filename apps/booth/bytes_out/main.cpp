#include <Arduino.h>

void setup(){
	Serial.begin(9600);
}

int main(){
	init();

	setup();

	uint8_t	i = 0;
	while(1){
		delay(100);
		i++;
		Serial.write(i);
	}
	return 0;
}