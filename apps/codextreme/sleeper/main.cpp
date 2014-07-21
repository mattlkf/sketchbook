#include <Arduino.h>

#define REEDPIN 3
#define CTRLPIN 13
#define FIXPIN 10
#define BASEPIN 11
#define STATUSPIN 12

#define ON_INTERVAL 100
#define OFF_INTERVAL 1000
#define CONST 0

uint32_t statusTimer;
uint32_t statusInterval;

uint32_t changeTimer;
uint8_t state;

uint32_t decayTimer;

#define FIX_TIMEOUT 60000
#define NOFIX_TIMEOUT 10000
uint32_t timeout;

uint8_t onoff;

uint32_t ticks;
#define TICKS_THRESHOLD 4

void setup(){
	pinMode(REEDPIN, INPUT_PULLUP);
	pinMode(CTRLPIN, OUTPUT);
	pinMode(FIXPIN, INPUT);
	pinMode(BASEPIN, INPUT);
	pinMode(STATUSPIN, OUTPUT);

	Serial.begin(9600);
	Serial.println("Ready");

	state = digitalRead(REEDPIN);

	onoff = 1;
	digitalWrite(CTRLPIN, HIGH);
	statusInterval = ON_INTERVAL;


	changeTimer = millis();
	decayTimer = millis();
	statusTimer = millis();
	return;
}


int main(){
	init();
	setup();

	while(1){
		if(statusInterval && millis() - statusTimer > statusInterval){
			digitalWrite(STATUSPIN, !digitalRead(STATUSPIN));
			statusTimer = millis();
		}

		if(millis() - decayTimer > 1000){
			decayTimer = millis();

			Serial.print("Ticks: ");
			Serial.print(ticks);
			Serial.print(" -> ");
			
			if(ticks > 2) ticks = ticks/2;

			Serial.println(ticks);
		}



		if(digitalRead(REEDPIN) != state){
			Serial.println("Tick!");
			state = digitalRead(REEDPIN);

			++ticks;
		}

		if(ticks > TICKS_THRESHOLD){
			onoff = 1;
			digitalWrite(CTRLPIN, HIGH);
			statusInterval = ON_INTERVAL;

			changeTimer = millis();
			Serial.print(ticks);
			Serial.println(": tick threshold met");
		}


		if(onoff == 1 && digitalRead(FIXPIN) == HIGH){
			if(timeout != FIX_TIMEOUT){
				timeout = FIX_TIMEOUT;
				Serial.println("Adjusted to Fix Timeout");
				statusInterval = CONST;
				digitalWrite(STATUSPIN, HIGH);
			}
		}
		else{
			if(timeout != NOFIX_TIMEOUT){
				timeout = NOFIX_TIMEOUT;
				Serial.println("Adjusted to No Fix Timeout");

				if(digitalRead(CTRLPIN) == LOW){
					statusInterval = OFF_INTERVAL;
				}
				else{
					statusInterval = ON_INTERVAL;
				}
			}
		}



		if(millis() - changeTimer > timeout && onoff == 1){
			onoff = 0;
			digitalWrite(CTRLPIN, LOW);
			statusInterval = OFF_INTERVAL;
			Serial.println("Reached timeout, turned off");
		}
	}

	return 0;
}