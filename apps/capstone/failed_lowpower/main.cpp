#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <Arduino.h>

void setup(){
  Serial.begin(115200);
  Serial.println("Ready!");
  pinMode(13, OUTPUT);
}

enum period_t
{
  SLEEP_15MS,
  SLEEP_30MS, 
  SLEEP_60MS,
  SLEEP_120MS,
  SLEEP_250MS,
  SLEEP_500MS,
  SLEEP_1S,
  SLEEP_2S,
  SLEEP_4S,
  SLEEP_8S,
  SLEEP_FOREVER
};

void idle(period_t period){

  // Save the relevant bits and turn off Timer 2
  uint8_t t2_mask = (1 << CS22) | (1 << CS21) | (1 << CS20);
  uint8_t saved_timer2_bits = TCCR2B & t2_mask;
  TCCR2B &= ~t2_mask;      
  power_timer2_disable();
  
  // Turn off ADC
  ADCSRA &= ~(1 << ADEN);
  power_adc_disable();
  
  power_timer1_disable(); 
  power_timer0_disable(); 
  power_spi_disable();
  power_usart0_disable();
  power_twi_disable();
  
  if (period != SLEEP_FOREVER)
  {
    wdt_enable(period);
    WDTCSR |= (1 << WDIE);  
  }
  
  // Enter sleep, do not disable brown-out detector
  set_sleep_mode(SLEEP_MODE_IDLE);
  cli();
  sleep_enable();
  sei();
  sleep_cpu();
  sleep_disable();
  sei();  

  power_twi_enable();
  power_usart0_enable();
  power_spi_enable();
  power_timer0_enable();  
  power_timer1_enable();  

  // Reenable ADC
  power_adc_enable();
  ADCSRA |= (1 << ADEN);

  // Reenable timer 2
  TCCR2B |= saved_timer2_bits;    
  power_timer2_enable();
}

int main(){
  init();
  setup();

  for(int i=0;1;i++){
    Serial.print("Hi ");
    Serial.println(i);

    digitalWrite(13, HIGH);
    delay(10);
    digitalWrite(13, LOW);    
    delay(2000);
    // idle(SLEEP_2S);
  }

  return 0;
}