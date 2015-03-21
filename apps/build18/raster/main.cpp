#include <Arduino.h>

#define STEP_X 3
#define DIR_X 6
#define STEP_Y 2
#define DIR_Y 5
#define ENABLE 8

#define SCALE_FACTOR 5

char blocked_read(){
    while(!Serial.available());
    return Serial.read();
}

void setup(){
    Serial.begin(9600);
    Serial.println("Ready");
    pinMode(STEP_X, OUTPUT);
    pinMode(DIR_X, OUTPUT);
    pinMode(STEP_Y, OUTPUT);
    pinMode(DIR_Y, OUTPUT);
    pinMode(ENABLE, OUTPUT);
    digitalWrite(ENABLE, HIGH);
    digitalWrite(STEP_X, LOW);
    digitalWrite(DIR_X, LOW);
    digitalWrite(STEP_Y, LOW);
    digitalWrite(DIR_Y, LOW);

    // wait for a single character to be sent over
    blocked_read();
    digitalWrite(ENABLE, LOW);
    Serial.println("Start!");
}

uint32_t scale(uint8_t val){
    return ((uint32_t) val) * SCALE_FACTOR;
}

void stepRight(){
    digitalWrite(DIR_X, HIGH);
    delayMicroseconds(2); // 10 times longer than required by datasheet
    digitalWrite(STEP_X, HIGH);
    delayMicroseconds(10); // 10 times longer than required by datasheet
    digitalWrite(STEP_X, LOW);
}
void stepLeft(){
    digitalWrite(DIR_X, LOW);
    delayMicroseconds(2); // 10 times longer than required by datasheet
    digitalWrite(STEP_X, HIGH);
    delayMicroseconds(10); // 10 times longer than required by datasheet
    digitalWrite(STEP_X, LOW);
}
void stepUp(){
    digitalWrite(DIR_Y, HIGH);
    delayMicroseconds(2); // 10 times longer than required by datasheet
    digitalWrite(STEP_Y, HIGH);
    delayMicroseconds(10); // 10 times longer than required by datasheet
    digitalWrite(STEP_Y, LOW);
}
void stepDown(){
    digitalWrite(DIR_Y, LOW);
    delayMicroseconds(2); // 10 times longer than required by datasheet
    digitalWrite(STEP_Y, HIGH);
    delayMicroseconds(10); // 10 times longer than required by datasheet
    digitalWrite(STEP_Y, LOW);
}

int main(){
    init();

    setup();
    while(1){
        Serial.write("A"); //request the next command

        uint8_t cmd = blocked_read();
        uint8_t val = blocked_read();

        if(cmd == 'r'){
            stepRight();
        }
        else if(cmd == 'l'){
            stepLeft();
        }
        else if(cmd == 'u'){
            stepUp();
        }
        else if(cmd == 'd'){
            stepDown();
        }

        if(val !='\n'){
            delayMicroseconds(100);
            delay(scale(val));
        }

    }
    return 0;
}