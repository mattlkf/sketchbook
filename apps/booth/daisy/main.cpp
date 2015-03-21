#include <Arduino.h>
#include <SoftwareSerial.h>

#define np 8
#define hysterisis 5
#define thresh 40

uint8_t pin[np] = {A0,A1,A2,A3,A4,A5,A6,A7};
uint16_t base[np];
uint8_t state[np];

void setup(){
	Serial.begin(9600);
	Serial.println("Hi!");

	uint8_t i;
	for(i=0;i<np;i++) state[i] = 0;
}

uint32_t diff(uint32_t a, uint32_t b){
	return (a>b) ? a-b : b-a;
}

void calibrate(){
	const uint8_t wlen = 6; // length of sliding window
	const uint32_t cthresh = 5; // calibration stabilization threshold

	uint16_t window[wlen][np]; // take wlen snapshots of np pins
	uint8_t i, j, k;

	// zero out array initially
	for(i=0;i<wlen;i++)
		for(j=0;j<np;j++)
			window[i][j] = 0;

	for(i=0;i<wlen;i = (i+1) % wlen){
		for(j=0;j<np;j++){
			window[i][j] = analogRead(pin[j]);
			Serial.print(window[i][j]);
			Serial.print('\t');
		}

		for(j=0;j<np;j++){
			uint32_t sum = 0;
			for(k=0;k<wlen;k++){
				sum += window[k][j];
			}

			if(diff((uint32_t) window[i][j] * (uint32_t) wlen, sum) > cthresh){
				j = np+1;
			}
		}

		if(j == np){
			Serial.println("Stable!");
			for(j=0;j<np;j++){
				base[j] = window[i][j];
			}
			break;
		}
		else{
			Serial.println();
		}
		
		delay(250);
	}
}


void send(uint8_t ch){
	// Serial.print("hi! ");
	// Serial.write(ch + '0');
	// Serial.println();
}

uint8_t on(uint16_t	read, uint16_t pin){
	uint16_t b = base[pin];

	if(state[pin] == 0){ // currently off
		return (read > b + thresh + hysterisis) || (read + thresh + hysterisis < b);
	}
	else{ // if currently on, the criteria to be considered still on is relaxed
		return (read > b + thresh) || (read + thresh < b);
	}
}

int main(){
	init();
	setup();

	calibrate();
	uint8_t i;

	Serial.println("---Calibrated baselines---");
	for(i=0;i<np;i++){
		Serial.print(base[i]);
		Serial.print("\t");
	}
	Serial.println();

	// delay(1000);

	while(1){
		for(i=0;i<np;i++){
			delay(10);
			uint16_t val = analogRead(pin[i]);

			// Serial.print(val);
			// Serial.print('\t');

			if(state[i] == 0 && on(val, i)){
				state[i] = 1;
				send(i*2);
				Serial.print("On: ");
				Serial.print(i);
				// Serial.print("Val: ");
				// Serial.print(val);
				// Serial.print(" Base: ");
				// Serial.print(base[i]);
				Serial.println();
			}
			else if(state[i] == 1 && !on(val, i)){
				state[i] = 0;
				send(i*2+1);
				Serial.print("Off: ");
				Serial.print(i);
				// Serial.print("Val: ");
				// Serial.print(val);
				// Serial.print(" Base: ");
				// Serial.print(base[i]);
				Serial.println();
			}
		}
		// Serial.println();
		delay(250);
	}
	return 0;
}