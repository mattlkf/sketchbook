#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2,3);

void setup(){
	mySerial.begin(9600);
	Serial.begin(9600);
}

char f(char c){
	if(c >= 'a' && c <= 'z')
		return c - ('a' - 'A');

	return c;
}

int main(){
	init();

	setup();

	while(1){
		if(mySerial.available()){
			Serial.write(f(mySerial.read()));

		}
	}

	return 0;
}