// A second version for two motors; code also reorganized

#include <Arduino.h>

#define PSTEP_X 2
#define PSTEP_Y 3
#define PDIR_X 5
#define PDIR_Y 6
#define PENABLE 8

struct motor_state{
    uint8_t dir;
    uint8_t on;
    uint8_t selected;
    uint16_t delay;
    uint32_t steps;
};

struct motor_state mx, my;

void flip(uint8_t &a){
    a = 1-a;
}

void setup(){
    Serial.begin(9600);
    Serial.println("Ready");
    pinMode(PSTEP_X, OUTPUT);
    pinMode(PSTEP_Y, OUTPUT);
    pinMode(PDIR_X, OUTPUT);
    pinMode(PDIR_Y, OUTPUT);
    pinMode(PENABLE, OUTPUT);

    digitalWrite(PENABLE, LOW);
    digitalWrite(PSTEP_X, LOW);
    digitalWrite(PSTEP_Y, LOW);
    digitalWrite(PDIR_X, LOW);
    digitalWrite(PDIR_Y, LOW);

    mx.selected = 0;
    my.selected = 0;
}

int main(){
    init();

    setup();
    while(1){
        if(!Serial.available()) continue;

        char ch = Serial.read();

        if(ch == 'x') flip(mx.selected);
    }
    return 0;
}