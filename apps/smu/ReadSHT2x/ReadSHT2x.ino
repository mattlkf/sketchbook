/****************************************************************
 * ReadSHT2x
 *  An example sketch that reads the sensor and prints the
 *  relative humidity to the PC's serial port
 *
 *  Tested with:
 *    - SHT21-Breakout Humidity sensor from elechouse.com

 ***************************************************************/

#include <Wire.h>
#include <SHT2x.h>


void setup()
{
  Wire.begin();
  Serial.begin(9600);
}

void loop()
{
  Serial.print("Humidity(%RH): ");
  Serial.println(SHT2x.GetHumidity());
  Serial.print("Temperature(C): ");
  Serial.println(SHT2x.GetTemperature());
  
  delay(1000);
}

