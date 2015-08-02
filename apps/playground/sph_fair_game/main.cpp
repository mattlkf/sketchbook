#include <Arduino.h>
#include <CapacitiveSensor.h>

/*
 * Capacitive "buzzers"!
 */

#define TIMEOUT -1

#define NS 3

#define QLEN 3

#define right_delta 10
#define wrong_delta -2

CapacitiveSensor   cs[NS] = {
		CapacitiveSensor(5,2), 
		CapacitiveSensor(5,3), 
		CapacitiveSensor(5,8), 
	};

int led[NS] = {10,11,12};
int i;
int buzzed = 0;
int buzz_index;

uint32_t lastBuzz = 0;

int score[QLEN][NS];
int qs; // queue start
int qe; // queue end

void reset_score(){
    qe = 0;
    qs = 0;
    int i;
    for(i=0;i<NS;i++) score[0][i] = 0;
}

void setup()                    
{
   for(i=0;i<NS;i++){
   	pinMode(led[i], OUTPUT);
   	// cs[i].set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate
   }
   reset_score();
   Serial.begin(9600);
}

void undo(){
    if(qe == qs){
        println("Cannot undo any more");
    }
    else{
        qe = (qe + QLEN - 1) % QLEN;
    }
}

void edit(int sel, int del){
    int prev[NS];
    int i;
    for(i=0;i<NS;i++){
        prev[i] = score[qe][i];
    }

    qe = (qe + 1) % QLEN;
    if(qe == qs) qs = (qs + 1) % QLEN;

    for(i=0;i<NS;i++){
        score[qe][i] = prev[i];
    }

    score[qe][sel] += del;
}

void loop()                    
{
	// if you have input, then reset and clear everything
	if(Serial.available()){
		// clear serial input buffer
		char ch;
		while(Serial.available()){
			ch = Serial.read();
            if(buzzed && ch == 'y'){
                edit(buzz_index, right_delta);
            }
            else if(buzzed && ch == 'n'){
                edit(buzz_index, wrong_delta);
            }
            else if(ch == 'u'){
                undo();
            }
            else if(ch == 'r'){
                reset_score();
            }
		}

		// turn off all lights
		for(i=0;i<NS;i++){
			digitalWrite(led[i], LOW);
		}

        // print the current score
		Serial.print("Score: ");
        for(i=0;i<NS;i++){
            Serial.print(score[qe][i]);
            Serial.print(" ");
        }
        Serial.println();

        buzzed = 0;
    }

    long reading[NS];
    for(i=0;i<NS;i++) reading[i] = cs[i].capacitiveSensor(30);

    for(i=0;i<NS && !buzzed;i++){
    	if(reading[i] > 250){
    		digitalWrite(led[i], HIGH);
    		buzzed = 1;
            buzz_index = i;

    		Serial.print("Buzzed! -- ");
    		Serial.println(i+1);
    		lastBuzz = millis();
    	}
    	else{
    		digitalWrite(led[i], LOW);
    	}
    }

}

int main(){
	init();

	setup();
	while(1){
		loop();
	}
	return 0;
}