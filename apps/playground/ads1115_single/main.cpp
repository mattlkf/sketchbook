#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads;  /* Use this for the 16-bit version */
// Adafruit_ADS1015 ads;     /* Use thi for the 12-bit version */

void setup(void)
{
  Serial.begin(9600);
  Serial.println("Hello!");
  
  Serial.println("ADC Range: (1 bit = 0.0078125mV, ADS1115)");
  
  // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
  
  ads.begin();
}

void loop(void)
{
  int16_t results;
  
  /* Be sure to update this value based on the IC and the gain settings! */
  // float   multiplier = 3.0F;    /* ADS1015 @ +/- 6.144V gain (12-bit results) */
  // float multiplier = 0.1875F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */

  float multiplier = 0.0078125F; /* ADS1115  @ +/- 6.144V gain (16-bit results) */
  float curr_mul = 0.015874; // empirically found
  results = ads.readADC_SingleEnded(0);  
    
  Serial.print("ADC_0: "); Serial.print(results); Serial.print("("); 
  Serial.print(results * multiplier); Serial.print("mV)");

  Serial.print(" Current : "); Serial.print(results * curr_mul); 
  Serial.println("mA");
  delay(200);
}

int main(){
	init();

	setup();
	while(1){
		loop();
	}
	return 0;
}
