#include <Arduino.h>

#define STEPPIN 3
#define DIRPIN 6
#define ENABLE 8

char dir = 0;

void setup(){
    Serial.begin(9600);
    Serial.println("Ready");
    pinMode(STEPPIN, OUTPUT);
    pinMode(DIRPIN, OUTPUT);
    pinMode(ENABLE, OUTPUT);
    digitalWrite(ENABLE, HIGH);
    digitalWrite(STEPPIN, LOW);
    digitalWrite(DIRPIN, LOW);
}

int main(){
    init();

    setup();
    while(1){
        if(Serial.available()){
            char ch = Serial.read();
            if(ch == 'e'){
                digitalWrite(ENABLE, LOW);
            }
            else if(ch == 'd'){
                digitalWrite(DIRPIN, dir ? LOW : HIGH);
                dir = 1 - dir;
            }
            else if(ch == ' '){
                digitalWrite(STEPPIN, HIGH);
                delay(1);
                digitalWrite(STEPPIN, LOW);
            }
            else if(ch == 's'){
                while(!Serial.available()){
                    digitalWrite(STEPPIN, HIGH);
                    delayMicroseconds(200);
                    digitalWrite(STEPPIN, LOW);
                    delayMicroseconds(200);
                }
            }
            else if(ch >= '1' && ch <='9'){
                ch -= '0';
                Serial.print("Delay: ");
                Serial.println(100 * ch * ch);
                while(1){
                    if(Serial.available()){
                        char x = Serial.read();
                        if(x == 'd'){
                               digitalWrite(DIRPIN, dir ? LOW : HIGH);
                               dir = 1 - dir;
                        }
                        else if(x >= '1' && x <= '9'){
                            ch = x - '0';
                        }
                        else break;
                    }
                    digitalWrite(STEPPIN, HIGH);
                    delayMicroseconds(100 * ch);
                    digitalWrite(STEPPIN, LOW);
                    delayMicroseconds(100 * ch);   
                }
            }
        }
    }
    return 0;
}