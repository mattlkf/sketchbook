#include <Arduino.h>

const uint8_t dir_left_pin = 4;
const uint8_t en_left_pin = 5;
const uint8_t en_right_pin = 6;
const uint8_t dir_right_pin = 7;

volatile uint32_t steps_left = 0;
volatile uint32_t steps_right = 0;

uint32_t prev_steps_left = 0;
uint32_t prev_steps_right = 0;

void left_isr()
{
  static uint32_t last = 0;
  if(millis() - last < 1) return;

  last = millis();

  steps_left++;
}

void right_isr(){
  static uint32_t last = 0;
  if(millis() - last < 1) return;

  last = millis();

  steps_right++;

}


void setup()
{
  Serial.begin(9600);
  pinMode(dir_left_pin, OUTPUT);
  pinMode(en_left_pin, OUTPUT);
  pinMode(en_right_pin, OUTPUT);
  pinMode(dir_right_pin, OUTPUT);

  digitalWrite(dir_left_pin, HIGH);
  digitalWrite(dir_right_pin, HIGH);

  digitalWrite(en_left_pin, LOW);
  digitalWrite(en_right_pin, LOW);

  attachInterrupt(0, left_isr, RISING);
  attachInterrupt(1, right_isr, RISING);

  Serial.println("ready");
}

void loop()
{
  if(steps_right > prev_steps_right || steps_left > prev_steps_left){
    Serial.print(steps_right);
    Serial.print(' ');
    Serial.println(steps_left);

    prev_steps_left = steps_left;
    prev_steps_right = steps_right;
  }

}



int main(){
    init();
    setup();

    while(1){
        loop();
    }
}