#include <Arduino.h>

const uint8_t dir_left_pin = 2;
const uint8_t en_left_pin = 5;
const uint8_t en_right_pin = 6;
const uint8_t dir_right_pin = 4;

uint8_t dir_left = 0;
uint8_t dir_right = 0;
uint8_t en_left = 0;
uint8_t en_right = 0;

uint8_t speed = 100;

uint32_t last_time;

char last_ch = 0;

void setup(){
    Serial.begin(9600);
    pinMode(dir_left_pin, OUTPUT);
    pinMode(en_left_pin, OUTPUT);
    pinMode(en_right_pin, OUTPUT);
    pinMode(dir_right_pin, OUTPUT);
    Serial.println("Ready");
}

void loop(){
    if(Serial.available()){
        char ch = Serial.read();
        // Serial.write(ch);
        // Serial.write('\t');

        last_time = millis();
        if(ch == ' '){
            if(speed == 255) speed = 100;
            else speed = 255;
        }
        else if(ch == last_ch){
            last_ch = 0;
            en_left = 0;
            en_right = 0;
        }
        else {
            last_ch = ch;

            if(ch == 'u'){
                dir_left = 0;
                dir_right = 0;
                en_left = 1;
                en_right = 1;
                // Serial.println("Forward");
            }
            else if(ch == 'd'){
                dir_left = 1;
                dir_right = 1;
                en_left = 1;
                en_right = 1;
                // Serial.println("Backward");
            }
            else if(ch == 'r'){ 
                dir_left = 1;
                dir_right = 0;
                en_left = 1;
                en_right = 1;
                // Serial.println("Left");
            }
            else if(ch == 'l'){
                dir_left = 0;
                dir_right = 1;
                en_left = 1;
                en_right = 1;
                // Serial.println("Right");
            }
         }
        Serial.println();
    }

    // if(millis() - last_time > 100){
    //     en_left = 0;
    //     en_right = 0;
    // }

    digitalWrite(dir_left_pin, dir_left);
    digitalWrite(dir_right_pin, dir_right);
    analogWrite(en_left_pin, en_left ? speed : 0);
    analogWrite(en_right_pin, en_right ? speed : 0);

}


int main(){
    init();

    setup();
    while(1){
        loop();
    }

    return 0;
}