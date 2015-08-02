#include <Arduino.h>

void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");
}

int main(){
	init();

	setup();
	pinMode(3, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	digitalWrite(3, HIGH);
	digitalWrite(5, HIGH);
	digitalWrite(9, HIGH);
	digitalWrite(10, HIGH);
	digitalWrite(11, HIGH);

	// char name[] = "Matthew";
	// while(1){
	// 	digitalWrite(13, LOW);
	// 	delay(1000);
	// 	digitalWrite(13, HIGH);
	// 	delay(100);
	// 	name[0]++;

	// 	Serial.println("Hi!");
	// }
	while(1);
	return 0;
}