/*
* Simple medical box: when it's on, it simply sends a packet once a second.
* A pared-down version of nrf_press_to_send
*/

#include <Arduino.h>
#include <SPI.h>
#include "RF24.h"

#include <Narcoleptic.h>

const uint16_t node_id = 0;

/****************** Packet definition *********************/
// sorry this isn't in a common file!
// also I know this is pretty primitive

struct med_packet{ // max 32 bytes in a packet
    uint16_t pkt_type;
    uint16_t src_id;
    uint32_t pkt_num;
};

/****************** User Config ***************************/
// Choose the names of the transmiting Pipe and the reading Pipe.
#define RXNAME "shine" // 5 bytes (unused)
#define TXNAME "shine" // 5 bytes

#define PAYLOADLEN 32

uint8_t data[PAYLOADLEN]; // a data buffer

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(A1,A0); //(ce,csn)(used to be 8,7)
/**********************************************************/

void setupRF24(){
    radio.begin();

    radio.setPALevel(RF24_PA_MAX);

    // Open a writing and reading pipe
    radio.openWritingPipe((byte *) TXNAME);
    radio.openReadingPipe(1, (byte *) RXNAME);

    // Start the radio listening for data
    radio.startListening();
    // radio.stopListening();
}

void setup() {
    Serial.begin(115200);

    setupRF24();
}

uint8_t send(){
    static uint32_t num_sent = 0;

    med_packet * p = (med_packet *) data; // make use of "data" as scratchpad
    p->pkt_type = 0; // medical box type
    p->src_id = node_id;
    p->pkt_num = num_sent++;

    // Write data
    // snprintf((char*)data, PAYLOADLEN, "Hello :) %lu", pkt_num++);

    // Stop listening so we can talk
    radio.stopListening();

    // Send data. This is a blocking call.
    uint8_t result = radio.write(data, PAYLOADLEN);

    // Resume listening
    radio.startListening();

    // Return the error code. If result is 0, there was a sending failure.
    return (result == 0); 
}

// Matt: the main function that Arduino appends (I think)

int main(){
    init();
    setup();

    while(1){
        uint32_t ts = millis();

        // Send a packet
        // verbose(send);
        send();

        // Flush serial
        Serial.flush();

        // Turn off the radio
        radio.powerDown();

        // Calculate how much of the second remains
        uint32_t elapsed = millis() - ts;

        // Sleep
        if(elapsed < 1000){
            Narcoleptic.delay(1000 - elapsed);
        }
    
        // Turn on the radio again
        radio.powerUp();
    }
    return 0;
}