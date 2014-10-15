#include <Arduino.h>

const uint8_t dir_left_pin = 2;
const uint8_t en_left_pin = 5;
const uint8_t en_right_pin = 6;
const uint8_t dir_right_pin = 4;

uint8_t dir_left = 0;
uint8_t dir_right = 0;
uint8_t en_left = 0;
uint8_t en_right = 0;

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
        Serial.write(ch);
        Serial.write('\t');
        if(ch == 'h'){
            dir_left = 1 - dir_left;
            Serial.print(dir_left);
        }
        else if(ch == 'j'){
            dir_right = 1 - dir_right;
            Serial.print(dir_right);
        }
        else if(ch == 'k'){
            en_right = 255 - en_right;
            Serial.print(en_right);
        }
        else if(ch == 'l'){
            en_left = 255 - en_left;
            Serial.print(en_left);
        }

        Serial.println();
    }

    digitalWrite(dir_left_pin, dir_left);
    digitalWrite(dir_right_pin, dir_right);
    analogWrite(en_left_pin, en_left);
    analogWrite(en_right_pin, en_right);

}


int main(){
    init();

    setup();
    while(1){
        loop();
    }

    return 0;
}