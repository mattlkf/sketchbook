/*
* A simple sender. Always listening, but press spacebar to send a packet.
* Modelled after nrf_press_to_send
*/

#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"

/****************** User Config ***************************/
// Choose the names of the transmiting Pipe and the reading Pipe.
#define RXNAME "shine" // 5 bytes
#define TXNAME "shine" // 5 bytes

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

void setup() {
    Serial.begin(115200);
    Serial.println(F("Press _spacebar_ to send a packet"));

    setupRF24();
}

uint8_t send(){
    // Write data
    snprintf((char*)data, PAYLOADLEN, "time: %lu", millis());

    // Stop listening so we can talk
    radio.stopListening();

    // Send data. This is a blocking call.
    uint8_t result = radio.write(data, PAYLOADLEN);

    // Resume listening
    radio.startListening();

    // Return the error code. If result is 0, there was a sending failure.
    return (result == 0); 
}

uint8_t received(){
    return radio.available();
}

void showdata(){
    uint8_t i;
    
// Get the data
    radio.read(data, PAYLOADLEN);

// For debugging: dump the 32 bytes' values
    Serial.print("Recv: ");
    for(i=0; i<PAYLOADLEN;i++){
        Serial.print(data[i]);
        Serial.write(' ');
    }
    Serial.println();

// Print the data (stopping at null terminator if any)
    for(i=0; data[i] && i<PAYLOADLEN; i++){
        Serial.write(data[i]);
    }
    Serial.println();
}

uint8_t pressed(){
    uint8_t pflag = 0;
    while(Serial.available()){ // empty the serial buffer
        pflag = 1;
        Serial.read();
    }
    return pflag;
}

void notify_error(uint8_t result){
    if(result){
        Serial.println("Error sending");
    }
}

int main(){
    init();
    setup();

    while(1){
        if(pressed()) notify_error(send());
        if(received()) showdata();
    }
    return 0;
}