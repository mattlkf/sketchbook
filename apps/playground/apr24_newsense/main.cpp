/* Calculations:
Pololu sensor: 185mV / A
ADS1115 at unity gain: 1bit = 0.125mV (this is volt_mult)
---
Resolution: 1bit = .125 / 185 * 1000 = 0.675675676 mA (this is cur_mult)

*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>
#include <SoftwareSerial.h>

#define VERBOSE

SoftwareSerial mySerial(2,3);

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */

/* Statistics for each run */
struct pkt_stats{
  uint16_t index;
  float current;
  float voltage;
  float power;
  uint32_t n;
  uint32_t init_time;
} stats;

/* Values for each snapshot */
float piVoltage;
float piCurrent;
float piPower;

void initStats(){
  stats.index++;
  stats.current = 0;
  stats.voltage = 0;
  stats.power = 0;
  stats.n = 0;
  stats.init_time = millis();
}

void setup(void)
{
  mySerial.begin(9600);
  Serial.begin(9600);

#ifdef VERBOSE
  Serial.println("Hello!");
#endif

  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.begin();

  initStats();
  stats.index = 0;
}

void measure(void)
{
  
  /* Be sure to update this value based on the IC and the gain settings! */
  int16_t r3 = ads.readADC_SingleEnded(3);  
  int16_t r1 = ads.readADC_SingleEnded(1);
  // results = ads.readADC_Differential_0_1();  
  float volt_mult = 0.125;
  float cur_mult = 0.675675676;

  piCurrent = r3 * cur_mult;
  piVoltage = r1 * volt_mult;
  piPower = piCurrent * piVoltage / 1000; // in mW

#ifdef VERBOSE  
  Serial.print("ADC_3: "); 
  Serial.print(r3);
  Serial.print("("); 
  Serial.print(r3 * volt_mult);
  Serial.print("mV)");
  
  Serial.print(" Current : "); 
#endif
  Serial.print(piCurrent);
  Serial.print(" ");

#ifdef VERBOSE
  Serial.print("mA");

  Serial.print(" Voltage : "); 
#endif
  Serial.print(piVoltage); 
  Serial.print(" ");

#ifdef VERBOSE
  Serial.print("mV");
#endif

  Serial.print(millis());

  Serial.print(" ");
  Serial.print(piPower);

  Serial.println();
}

void printStats(){
  Serial.print("Stats: ");
  Serial.print(stats.index);
  Serial.print(" ");
  Serial.print(stats.current / (float)stats.n);
  Serial.print(" ");
  Serial.print(stats.voltage / (float)stats.n);
  Serial.print(" ");
  Serial.print(stats.power / (float)stats.n);
  Serial.print(" ");
  Serial.print(millis() - stats.init_time);
  Serial.println();
}

void updateStats(){
  stats.current += piCurrent;
  stats.voltage += piVoltage;
  stats.power += piPower;
  ++stats.n;
}

int main(){
	init();

	setup();

  const uint32_t delay = 1000;
  uint32_t t = millis();
	while(1){
    if(millis() - t > delay){
      t = millis();
      measure();
      updateStats();
    }

    if(mySerial.available()){
      printStats();

      while(1){
        if(mySerial.available()){
          char ch = mySerial.read();
          Serial.write(ch);

          if(ch == '\n') break;          
        }
      }

      initStats();
    }

	}
	return 0;
}
