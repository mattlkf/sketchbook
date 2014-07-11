#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>

void setup(){
	Serial.begin(9600);
}

int main(){
	init();
	setup();

	while(1){


	}

	return 0;
}