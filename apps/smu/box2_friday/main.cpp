#include <Arduino.h>

// NARCO
#include <Narcoleptic.h>

// SHT21
#include <Wire.h>
#include <SHT2x.h> 

// RF24
#include <SPI.h>
#include "RF24.h"

// OLED
#include <GOFi2cOLED.h>

// Define the payload format
struct packet{
    float t, h;
};

#define DELAY_MILLISECONDS 2000 // 10 seconds

// #define LEDPIN 13 // we shouldn't be using the LED anyway, it's for the NRF
#define SHT_PIN 5
#define NRF_PIN 10 // not to be powered down
#define OLED_PIN 6 // this should never be powered down

#define PRINT_TO_SERIAL

uint32_t pkt_num = 0;

/****************** User Config ***************************/
// Choose the names of the transmiting Pipe and the reading Pipe.
#define RXNAME "serv1" // 5 bytes
#define TXNAME "serv1" // 5 bytes

#define PAYLOADLEN 32

uint8_t data[PAYLOADLEN]; // a data buffer

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8,7); //(ce,csn)

GOFi2cOLED oled;

/**********************************************************/

void busymode();

void setupRF24(){
    radio.begin();

    // Set the PA Level low to prevent power supply related issues since this is a
    // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
    // radio.setPALevel(RF24_PA_LOW);
    radio.setPALevel(RF24_PA_MAX);

    // Open a writing and reading pipe
    radio.openWritingPipe((byte *) TXNAME);
    radio.openReadingPipe(1, (byte *) RXNAME);

    // Start the radio listening for data
    radio.startListening();

    radio.powerDown();
}

void setupOLED(){
    pinMode(OLED_PIN, OUTPUT);
    digitalWrite(OLED_PIN, HIGH);

    delay(100); // give the hardware some time to init

    oled.init(0x3C);  //initialze  OLED display, default slave address is 0x3D

    oled.display(); // splash screen

    delay(2000);

    oled.clearDisplay();

    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(0,0);

    oled.display();

    oled.sendCommand(GOFi2cOLED_Display_Off_Cmd);    /*display OFF*/

    delay(50);
}

void setup(){
    pinMode(NRF_PIN, OUTPUT);
    digitalWrite(NRF_PIN, HIGH);

    pinMode(SHT_PIN, OUTPUT);
    digitalWrite(SHT_PIN, HIGH);

    pinMode(3, INPUT);

    delay(100);

    Wire.begin();

    setupRF24();

    setupOLED();

    // Narcoleptic.disableMillis();
    // Narcoleptic.disableTimer1();
    // Narcoleptic.disableTimer2();
    #ifndef PRINT_TO_SERIAL 
        Narcoleptic.disableSerial(); // if we are not using serial at all, then disable it from the start
    #endif
    Narcoleptic.disableADC();
    // Narcoleptic.disableWire();
    // Narcoleptic.disableSPI();

    Narcoleptic.useInterrupt(1, FALLING);

    digitalWrite(SHT_PIN, LOW);
}

uint8_t send(uint8_t * data_pointer){
    // Stop listening so we can talk
    radio.stopListening();

    // Send data. This is a blocking call.
    uint8_t result = radio.write(data_pointer, PAYLOADLEN);

    // Resume listening
    radio.startListening();

    // Return the error code. If result is 0, there was a sending failure.
    return (result == 0); 
}

uint32_t awake_timer; // just for interest

int main(){
    init();
    setup();

    while(1){
        #ifdef PRINT_TO_SERIAL
            // Turn off serial during sleep.. ?
            Serial.flush();
            Serial.end();
        #endif
        

        // Disable stuff needed during awake-time
        #ifdef PRINT_TO_SERIAL
            Narcoleptic.disableSerial();
        #endif
        Narcoleptic.disableMillis();
        Narcoleptic.disableWire();

        // this is necessary to bring current consumption down from 0.35mA to 0.15mA in sleep
        digitalWrite(A4, LOW); // SDA low
        digitalWrite(A5, LOW); // SCL low

        // Sleep
        Narcoleptic.delay(DELAY_MILLISECONDS);
        // Re-enable stuff needed during awake-time
        Narcoleptic.enableWire(); // i2c
        Narcoleptic.enableMillis(); // without this, i2c doesn't work

        // for timing / diagnostics
        awake_timer = millis();

        #ifdef PRINT_TO_SERIAL
            Narcoleptic.enableSerial(); // printing
        #endif

        #ifdef PRINT_TO_SERIAL
        // Re-start serial
        Serial.begin(115200);
        #endif


        if(Narcoleptic.wasInterrupted()){
            // Print something if we woke up due to an interrupt
            #ifdef PRINT_TO_SERIAL
                Serial.println("Interrupt!");
            #endif

        // Enter a new state
            busymode();
        }
        else{
            // Read and print if we woke up without interrupt

        // Take a set of readings 
            digitalWrite(SHT_PIN, HIGH);
            float h = SHT2x.GetHumidity();
            float t = SHT2x.GetTemperature();
            digitalWrite(SHT_PIN, LOW);
        
        // Serial
            #ifdef PRINT_TO_SERIAL
                // Print to serial
                Serial.print("Humidity(%RH): ");
                Serial.println(h);
                Serial.print("Temperature(C): ");
                Serial.println(t);
            #endif


        // RF24 stuff
            // Write data in packet format
            packet p;
            p.t = t;
            p.h = h;

            snprintf((char*)data, PAYLOADLEN, "T: %f RH: %f (%u)", (double) t, (double) h, pkt_num++);

            // Turn on the radio
            radio.powerUp();

            // Send!!
            send((uint8_t *) data);
            // send((uint8_t *) &p);

            // Turn off the radio
            radio.powerDown();

        // Serial
            #ifdef PRINT_TO_SERIAL
                Serial.print("Awake: ");
                Serial.println(millis() - awake_timer);
            #endif
        }
    }

    return 0;
}

#define BUSY_MODE_TIMEOUT 15000

// This is like another "program" that runs when the button is pressed.
void busymode(){
// Set the humidity sensor HIGH so it doesn't affect the comms with the screen
    digitalWrite(SHT_PIN, HIGH);
    delay(100);

// Turn on the display
    oled.sendCommand(GOFi2cOLED_Display_On_Cmd);    /*display ON*/
    delay(100);
// Show something on the display
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.print("Are you ok?");
    oled.display();

    delay(2000);

// Display data
    uint32_t start_time = millis();
    while(millis() - start_time < BUSY_MODE_TIMEOUT){
    // Take a set of readings 
        // digitalWrite(SHT_PIN, HIGH);
        float h = SHT2x.GetHumidity();
        float t = SHT2x.GetTemperature();
        // digitalWrite(SHT_PIN, LOW);
        
        oled.clearDisplay();
        oled.setCursor(0,0);
        oled.print("RH %:");
        oled.print(h);
        oled.print("Temp:");
        oled.print(t);
        oled.display(); 

        delay(500);
    }

// Display bye-bye message
    oled.clearDisplay();
    oled.setCursor(0,0);
    oled.print("I'm going back to sleep!");
    oled.display();

    delay(2000);

// Turn off the screen
    oled.sendCommand(GOFi2cOLED_Display_Off_Cmd);    /*display OFF*/

// Turn off the SHT
    delay(100);
    digitalWrite(SHT_PIN, LOW);
}