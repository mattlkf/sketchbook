#include <Arduino.h>
#include <Narcoleptic.h>

#include <Wire.h>
#include <SHT2x.h> 

#include <SPI.h>
#include "RF24.h"

struct packet{
    float t, h;
    uint32_t pkt_num;
    uint32_t src_id;
};

#define SRC_ID 0

#define LEDPIN 13
#define SHT_PIN 10
#define NRF_PIN 9

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
/**********************************************************/

void setupRF24(){
    radio.begin();

    // Set the PA Level low to prevent power supply related issues since this is a
    // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
    radio.setPALevel(RF24_PA_LOW);

    // Open a writing and reading pipe
    radio.openWritingPipe((byte *) TXNAME);
    radio.openReadingPipe(1, (byte *) RXNAME);

    // Start the radio listening for data
    radio.startListening();
}


void setup(){
    pinMode(NRF_PIN, OUTPUT);
    digitalWrite(NRF_PIN, HIGH);

    pinMode(LEDPIN, OUTPUT);
    digitalWrite(LEDPIN, LOW);

    pinMode(SHT_PIN, OUTPUT);
    digitalWrite(SHT_PIN, LOW);

    Wire.begin();

    setupRF24();

    // Narcoleptic.disableMillis();
    // Narcoleptic.disableTimer1();
    // Narcoleptic.disableTimer2();
    #ifndef PRINT_TO_SERIAL 
        Narcoleptic.disableSerial(); // if we are not using serial at all, then disable it from the start
    #endif
    Narcoleptic.disableADC();
    // Narcoleptic.disableWire();
    // Narcoleptic.disableSPI();

    // Narcoleptic.useInterrupt(1, FALLING);
    digitalWrite(SHT_PIN, LOW);

}

void blink(uint8_t k){
    for(uint8_t i = 0; i < k;i++){
        digitalWrite(LEDPIN, HIGH);
        delayMicroseconds(1000);
        
        digitalWrite(LEDPIN, LOW);
        if(i < k) delay(999);
    }
    return;
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

uint32_t awake_timer;

int main(){
    init();
    setup();

    uint8_t i = 1;
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
        Narcoleptic.delay(1000);
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
            blink(3);
        }
        else{
            // Read and print if we woke up without interrupt

            // Take a set of readings 
            digitalWrite(SHT_PIN, HIGH);
            float h = SHT2x.GetHumidity();
            float t = SHT2x.GetTemperature();
            digitalWrite(SHT_PIN, LOW);
            
            #ifdef PRINT_TO_SERIAL
                // Print to serial
                Serial.print("Humidity(%RH): ");
                Serial.println(h);
                Serial.print("Temperature(C): ");
                Serial.println(t);
            #endif


        // RF24 stuff
            // Write data
            packet p;
            p.t = t;
            p.h = h;
            p.pkt_num = pkt_num++;
            p.src_id = SRC_ID;

            // snprintf((char*)data, PAYLOADLEN, "T: %f RH: %f (%d)", t, h, pkt_num++);

            // Turn on the radio
            radio.powerUp();

            send((uint8_t *) &p);

            // Turn off the radio
            radio.powerDown();

            Serial.print("Awake: ");
            Serial.println(millis() - awake_timer);

        }
    }

    return 0;
}