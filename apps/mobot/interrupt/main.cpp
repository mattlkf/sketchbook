#include <Arduino.h>

int pin = 13;
volatile int state = LOW;
volatile int cnt = 0;
int prev = 0;

const uint8_t dir_left_pin = 4;
const uint8_t en_left_pin = 6;

void onMotor(){
    analogWrite(en_left_pin, 100);
}

void offMotor(){
    analogWrite(en_left_pin, 0);
}

void blink()
{
  static uint32_t lastBlink = 0;
  if(millis() - lastBlink < 1) return;

  lastBlink = millis();

  cnt++;
  state = !state;
}

void setup()
{
    Serial.begin(9600);
    pinMode(pin, OUTPUT);

    pinMode(dir_left_pin, OUTPUT);
    pinMode(en_left_pin, OUTPUT);
    digitalWrite(dir_left_pin, 1);
    offMotor();

    attachInterrupt(0, blink, RISING);
    Serial.println("ready");
    
    onMotor();

}

void loop()
{
  digitalWrite(pin, state);
  if(cnt > prev){
    Serial.println(cnt);
    prev = cnt;
  }

  if(cnt > 800) offMotor();

}



int main(){
    init();
    setup();

    while(1){
        loop();
    }
}