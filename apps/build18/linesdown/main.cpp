// This program receives instructions from the PC.
// Each instruction is in the form of a pkt defined below.
// One segment <=> One instruction
// Todo: Reduce jerkiness between instructions
// Adjust speed
// Adjust scaling

#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(11, 12);

#define STEP_X 3
#define DIR_X 6
#define STEP_Y 2
#define DIR_Y 5
#define ENABLE 8
#define LASER 13

#define KNOB A1

#define DELAY 2500
#define DEL_CORRECTED 100 //works well with DELAY 2500
// 6970, 6970 - 2400 seems ok to
#define LAG 300
struct pkt{
    uint8_t cmd;
    uint8_t laser;
    uint16_t steps;
};

inline void sblock(){
    while(!Serial.available());
}

void readPkt(char * a){
    uint8_t i;
    for(i=0;i<sizeof(struct pkt);i++){
        sblock(); // wait until next character received
        a[i] = Serial.read();
    }
}

void setup(){
    Serial.begin(57600);
    mySerial.begin(9600);
    mySerial.println("Ready");

    pinMode(STEP_X, OUTPUT);
    pinMode(DIR_X, OUTPUT);
    pinMode(STEP_Y, OUTPUT);
    pinMode(DIR_Y, OUTPUT);
    pinMode(ENABLE, OUTPUT);
    pinMode(LASER, OUTPUT);
    digitalWrite(LASER, LOW); // this turns off the laser
    digitalWrite(ENABLE, HIGH); // this turns off the stepper drivers
    digitalWrite(STEP_X, LOW);
    digitalWrite(DIR_X, LOW);
    digitalWrite(STEP_Y, LOW);
    digitalWrite(DIR_Y, LOW);
}

void step(uint8_t dir){
    // 1: X CCW
    // 2: X CW
    // 3: Y CCW
    // 4: Y CW
    digitalWrite((dir < 2) ? DIR_X : DIR_Y, (dir % 2) ? HIGH : LOW);
    delayMicroseconds(2);
    digitalWrite((dir < 2) ? STEP_X : STEP_Y, HIGH);
    delayMicroseconds(10);
    digitalWrite((dir < 2) ? STEP_X : STEP_Y, LOW);
}

int main(){
    init();

    setup();

    while(1){
//        uint32_t tt = micros();
        Serial.write('A');
        
        uint16_t i;
        pkt a;
        readPkt((char *) &a);
        // mySerial.println(micros() - tt);
        // mySerial.println("---");
        // mySerial.println(a.cmd);
        // mySerial.println(a.laser);
        // mySerial.println(a.steps);
        // delay(1000);

        digitalWrite(LASER, a.laser ? HIGH : LOW);
        
        if(a.cmd < 4){
            // uint16_t deli = DELAY + 10 * analogRead(KNOB);
            for(i=0;i+1<a.steps;i++){
                step(a.cmd);
                // delayMicroseconds(DELAY + 5*analogRead(KNOB));
                // delayMicroseconds(deli);
                delayMicroseconds(DELAY);
            }
            step(a.cmd);
            // uint32_t del_corrected = analogRead(KNOB);
            delayMicroseconds(DEL_CORRECTED);
            // delayMicroseconds(deli - 2400);
            // mySerial.println(deli);
            // mySerial.println(del_corrected);          
        }
        else if(a.cmd == 4){
            digitalWrite(ENABLE, LOW);
            delay(100);
        }
        else if(a.cmd == 5){
            digitalWrite(ENABLE, HIGH);
            digitalWrite(LASER, LOW); // just in case
            mySerial.println("Finished");
            delay(1000);
        }
        else {
            mySerial.println("?");
        }
    }

    return 0;
}

// The hard-coded way of writing step()

// if(dir == 0){
    //     digitalWrite(DIR_X, LOW);
    //     delayMicroseconds(20);
    //     digitalWrite(STEP_X, HIGH);
    //     delayMicroseconds(10);
    //     digitalWrite(STEP_X, LOW);    
    // }
    // else if(dir == 1){
    //     digitalWrite(DIR_X, HIGH);
    //     delayMicroseconds(20);
    //     digitalWrite(STEP_X, HIGH);
    //     delayMicroseconds(10);
    //     digitalWrite(STEP_X, LOW);    
    // }
    // else if(dir == 2){
    //     digitalWrite(DIR_Y, LOW);
    //     delayMicroseconds(20);
    //     digitalWrite(STEP_Y, HIGH);
    //     delayMicroseconds(10);
    //     digitalWrite(STEP_Y, LOW);    
    // }
    // else if(dir == 3){
    //     digitalWrite(DIR_Y, HIGH);
    //     delayMicroseconds(20);
    //     digitalWrite(STEP_Y, HIGH);
    //     delayMicroseconds(10);
    //     digitalWrite(STEP_Y, LOW);    
    // }

