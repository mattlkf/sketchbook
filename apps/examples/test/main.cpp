#include <Arduino.h>
#include <Narcoleptic.h>

int array[100] = {0};

void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");
}

int main(){
	init();

	setup();
	pinMode(13, OUTPUT);
	int a = 1, b = 2, c = 3;
	char name[] = "Matthew";
	while(1){
		digitalWrite(13, LOW);
		delay(500);
		digitalWrite(13, HIGH);
		delay(100);
		name[0]++;

		Serial.println("Hi!");
	}
}