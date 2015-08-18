/**
 * Narcoleptic - A sleep library for Arduino
 * Copyright (C) 2010 Peter Knight (Cathedrow)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Narcoleptic_h
#define Narcoleptic_h

#include <inttypes.h>
#include <stdbool.h>

class NarcolepticClass
{
  public:
    void delay(int milliseconds);
    uint32_t millis();

    void disableWire();
    void disableTimer2();
    void disableTimer1();
    void disableMillis();
    void disableSerial();
    void disableADC();
    void disableSPI();

    void enableWire();
    void enableTimer2();
    void enableTimer1();
    void enableMillis();
    void enableSerial();
    void enableADC();
    void enableSPI();

// Matt's additions
    void useInterrupt(uint8_t, uint8_t); // enable wake-on-interrupt. First arg must be 0 or 1.
    uint8_t wasInterrupted(); // call this after delay to detect the reason for wake-up.

  private:
    void sleep(uint8_t,uint8_t);
    void calibrate();

    void setInterrupt();
    uint8_t isr_num, isr_mode;
// Matt's additions
};
extern NarcolepticClass Narcoleptic;

#endif
