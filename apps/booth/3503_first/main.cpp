#include <Arduino.h>

#define IN A0
#define IN2 A1

void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");
}

int main(){
	init();

	uint16_t t1,t2;
	t1 = 500;
	t2 = 525;

	setup();
	while(1){
		uint16_t val = analogRead(IN);
		Serial.println(val);
		
		t1 = 512 - analogRead(IN2)/20;
		t2 = 512 + analogRead(IN2)/20;
		Serial.print("[");
		Serial.print(t1);
		Serial.print(",");
		Serial.print(t2);
		Serial.println("]");

		if(val < t1){
			Serial.println("Hi!");
		}
		else if(val > t2){
			Serial.println("Bye!");
		}

		delay(500);
	}
	return 0;
}