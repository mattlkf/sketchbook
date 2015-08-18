#include <Arduino.h>

#define INPIN 11
#define OUTPIN 13

void setup(){
}

int main(){
	init();
	setup();

	pinMode(OUTPIN, OUTPUT);
	pinMode(INPIN, INPUT_PULLUP);	
	while(1){
		if(digitalRead(INPIN) == LOW){
			digitalWrite(OUTPIN, LOW);
		}
		else{
			digitalWrite(OUTPIN, HIGH);
		}
	}
}