#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void setup(){

	Serial.begin(115200);
	Serial.println("Accelerometer Test"); Serial.println("");

	if(!accel.begin()){
		Serial.println("Failed to init sensor");
		while(1);
	}


	accel.setRange(ADXL345_RANGE_4_G);


	return;
}

int main(){
	init();
	setup();

	while(1){

		/* Get a new sensor event */ 
		sensors_event_t event; 
		accel.getEvent(&event);

		/* Display the results (acceleration is measured in m/s^2) */
		Serial.print("X: "); Serial.print(event.acceleration.x); Serial.print("  ");
		Serial.print("Y: "); Serial.print(event.acceleration.y); Serial.print("  ");
		Serial.print("Z: "); Serial.print(event.acceleration.z); Serial.print("  ");Serial.println("m/s^2 ");
		delay(500);
	}
	return 0;
}