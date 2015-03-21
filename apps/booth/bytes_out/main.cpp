#include <Arduino.h>

void setup(){
	Serial.begin(115200);
}

int main(){
	init();

	setup();

	uint8_t	i = 60;
	while(1){
		delay(1000);
		i++;
		Serial.write(i);
	}
	return 0;
}