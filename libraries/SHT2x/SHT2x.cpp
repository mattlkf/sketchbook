/*
  SHT2x - A Humidity Library for Arduino.

  Supported Sensor modules:
    SHT21-Breakout Module - http://www.elechouse.com/elechouse/index.php?main_page=product_info&cPath=152_158&products_id=2235
	
  Created by Christopher Ladden at Modern Device on December 2009.
  Modified by Paul Badger March 2010

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <inttypes.h>
#include <Wire.h>
#include "Arduino.h"
#include "SHT2x.h"  

// #define SOFTI2C

#ifdef SOFTI2C
  #include "SoftI2CMaster.h" //matt
  #define I2C i2c //Matt (Choose between "Wire" and "SoftI2C" )

  const byte sdaPin = 4;
  const byte sclPin = 5;
  SoftI2CMaster i2c = SoftI2CMaster( sdaPin,sclPin );

#else
  #define I2C Wire
#endif

/******************************************************************************
 * Global Functions
 ******************************************************************************/

/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
float SHT2xClass::GetHumidity(void)
{
	return (-6.0 + 125.0 / 65536.0 * (float)(readSensor(eRHumidityHoldCmd)));
}

/**********************************************************
 * GetTemperature
 *  Gets the current temperature from the sensor.
 *
 * @return float - The temperature in Deg C
 **********************************************************/
float SHT2xClass::GetTemperature(void)
{
	return (-46.85 + 175.72 / 65536.0 * (float)(readSensor(eTempHoldCmd)));
}


/******************************************************************************
 * Private Functions
 ******************************************************************************/

uint16_t SHT2xClass::readSensor(uint8_t command)
{
    uint16_t result;

    I2C.beginTransmission(eSHT2xAddress);	//begin
    I2C.write(command);					//send the pointer location
    delay(100);
    I2C.endTransmission();               	//end



#ifndef SOFTI2C
    I2C.requestFrom(eSHT2xAddress, 3);

    uint16_t cyc = 0;
    while(I2C.available() < 3 && cyc < 10000) {
      ; //wait
      cyc++;
    }
#else
    I2C.requestFrom(eSHT2xAddress);    
#endif

    uint16_t a = 0;
    a = I2C.read();

    // delay(1);
    // Serial.print("a: ");
    // Serial.print(a);
    // Serial.println();

    uint16_t b = 0;
    b = I2C.read();


    // delay(1);
    // Serial.print("b: ");
    // Serial.print(b);
    // Serial.println();
    

    //Store the result
    result = a << 8;
    result += b;
	result &= ~0x0003;   // clear two low bits (status bits)

    // Serial.print("Result: ");
    // Serial.print(result);
    // Serial.println();

    return result;
}

SHT2xClass SHT2x;
