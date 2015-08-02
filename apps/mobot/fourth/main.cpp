#include <Arduino.h>
#include <SoftwareSerial.h>

const uint8_t dir_left_pin = 4;
const uint8_t en_left_pin = 3;
const uint8_t en_right_pin = 6;
const uint8_t dir_right_pin = 7;

uint8_t dir_left = 0;
uint8_t dir_right = 0;
uint8_t en_left = 0;
uint8_t en_right = 0;

uint8_t sr = 0, sl = 0;

uint8_t speed = 100;

uint32_t last_time;

char last_ch = 0;

int state = 0;

SoftwareSerial SS(10,11);

void setup(){
    SS.begin(9600);
    Serial.begin(9600);
    pinMode(dir_left_pin, OUTPUT);
    pinMode(en_left_pin, OUTPUT);
    pinMode(en_right_pin, OUTPUT);
    pinMode(dir_right_pin, OUTPUT);
    Serial.println("Ready");
}

void loop(){
    int i;

    if(Serial.available()){
        uint8_t buf[4];
        for(i=0;i<4;i++){
            while(!Serial.available());
            buf[i] = Serial.read();
        }

        float f = (float) buf;
        Serial.print("Read ");
        Serial.println(f);
    }


     
    digitalWrite(dir_left_pin, dir_left);
    digitalWrite(dir_right_pin, dir_right);
    analogWrite(en_left_pin, sl);
    analogWrite(en_right_pin, sr);

    if(state == 0 && analogRead(A0) > 500){
        state = 1;
        Serial.print('v');
    }
    else if(state == 1 && analogRead(A0) < 480){
        state = 0;
        Serial.print('w');
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