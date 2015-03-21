#include <Arduino.h>
#include <SoftwareSerial.h>

// #define VERBOSE

#define np 8
#define hysterisis 5
#define thresh 40

uint8_t pin[np] = {A0,A1,A2,A3,A4,A5,A6,A7};
uint16_t base[np];
uint8_t state[np];

void setup(){
	Serial.begin(57600);

	uint8_t i;
	for(i=0;i<np;i++) state[i] = 0;
	
#ifdef VERBOSE
	Serial.println("Hi!");
#endif
}

uint32_t diff(uint32_t a, uint32_t b){
	return (a>b) ? a-b : b-a;
}

void calibrate(){
	const uint8_t wlen = 6; // length of sliding window
	const uint32_t cthresh = 5; // calibration stabilization threshold
	const uint16_t sample_period = 250; // delay between successive calib. measurements

	uint16_t window[wlen][np]; // take wlen snapshots of np pins
	uint8_t i, j, k;

	// zero out array initially
	for(i=0;i<wlen;i++)
		for(j=0;j<np;j++)
			window[i][j] = 0;

	for(i=0;i<wlen;i = (i+1) % wlen){
		for(j=0;j<np;j++){
			window[i][j] = analogRead(pin[j]);

#ifdef VERBOSE
			Serial.print(window[i][j]);
			Serial.print('\t');
#endif
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
			for(j=0;j<np;j++){
				base[j] = window[i][j];
			}
#ifdef VERBOSE
			Serial.println("Stable!");
#endif
			break;
		}
		else{
#ifdef VERBOSE
			Serial.println();
#endif
		}	
		delay(sample_period);
	}
}


void send(uint8_t ch){
	Serial.write(ch);
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

#ifdef VERBOSE
	Serial.println("---Calibrated baselines---");
	for(i=0;i<np;i++){
		Serial.print(base[i]);
		Serial.print("\t");
	}
	Serial.println();
#endif

	while(1){
		for(i=0;i<np;i++){
			delay(10);
			uint16_t val = analogRead(pin[i]);

			// Serial.print(val);
			// Serial.print('\t');

			if(state[i] == 0 && on(val, i)){
				state[i] = 1;
				send(i*2);

#ifdef VERBOSE
				Serial.print("On: ");
				Serial.println(i);
#endif
			}
			else if(state[i] == 1 && !on(val, i)){
				state[i] = 0;
				send(i*2+1);

#ifdef VERBOSE
				Serial.print("Off: ");
				Serial.println(i);
#endif
			}
		}
		delay(250);
	}
	return 0;
}