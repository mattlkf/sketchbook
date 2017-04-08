#include <Arduino.h>
#include "Wire.h"
#include "SparkFunISL29125.h"

#define UART_TO_LED 0
#define COLOR_TO_UART 1
#define COLOR_TO_LED 2

#define LED_PIN 9

#define MAX_BUF 50

int state;
uint8_t val;

// Declare sensor object
SFE_ISL29125 RGB_sensor;

uint8_t timer_trigger(){
    static uint32_t last_time = 0;
    const uint32_t delay = 250; // the period
    if(millis() - last_time > delay){
        last_time = millis();
        return true;
    }
    return false;
}

void setup(){
  state = UART_TO_LED;

  analogWrite(LED_PIN, 0);
  val = 0;

  RGB_sensor.init();

  Serial.begin(115200);
  Serial.println("Ready!");
}

int main(){
  init();
  setup();

  int mr, mg, mb;
  mr = mg = mb = 0;

  while(1){
    while(Serial.available()){
      char ch = Serial.read();
      // Serial.println("Read");
      // Serial.println((uint8_t)ch);
      if(ch >= '0' && ch <= '9'){
        val *= 10;
        val += (ch - '0');
        // Serial.print("Added");
        Serial.print(ch);
      }
      else{
        if(ch == '\r'){
          Serial.print(ch);
          if(state == UART_TO_LED){
            Serial.print("Wrote to LED: ");
            Serial.println(val);
            analogWrite(LED_PIN, val);
            val = 0;
          }
        }
        else{
          if(ch == 'a') state = UART_TO_LED;
          if(ch == 'b') state = COLOR_TO_UART;
          if(ch == 'c') state = COLOR_TO_LED;
        }
      }
    }

    uint16_t red, green, blue;

    if(state == COLOR_TO_LED || state == COLOR_TO_UART){
      red   = RGB_sensor.readRed();
      green = RGB_sensor.readGreen();
      blue  = RGB_sensor.readBlue();      

      mr = max(red, mr);
      mg = max(green, mg);
      mb = max(blue, mb);
    }

    if(state == COLOR_TO_LED){
      
      float normr = (float)red / (float)mr;
      float normg = (float)green / (float)mg;
      float normb = (float)blue / (float)mb;

      float avg = (normr + normg + normb)/ 3;
      val = (uint8_t) (avg * 255);     
      // Serial.print("avg: ");
      // Serial.println(avg);
      // Serial.print("Wrote to LED: ");
      // Serial.println(val);
      analogWrite(LED_PIN, val);
    }

    if((state == COLOR_TO_UART || state == COLOR_TO_LED) && timer_trigger()){

      Serial.print("Color (RGB): ");
      Serial.print(red);
      Serial.print(" ");
      Serial.print(green);
      Serial.print(" ");
      Serial.print(blue);
      Serial.println();
    }
  }

  return 0;
}