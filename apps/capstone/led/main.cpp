/*
* A simple sender. Always listening, but press spacebar to send a packet.
* Modelled after nrf_press_to_send
*/

// #include <Arduino.h>
#include <SPI.h>
#include "RF24.h"
#include "Wire.h"
#include "SparkFunISL29125.h"

/****************** User Config ***************************/
// Choose the names of the transmiting Pipe and the reading Pipe.
#define RXNAME "serv1" // 5 bytes
#define TXNAME "serv1" // 5 bytes

#define PAYLOADLEN 32

uint8_t data[PAYLOADLEN]; // a data buffer

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8,7); //(ce,csn)
/**********************************************************/

// Declare sensor object
SFE_ISL29125 RGB_sensor;

void setupRF24(){

    radio.begin();

    // Set the PA Level low to prevent power supply related issues since this is a
    // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
    // radio.setPALevel(RF24_PA_MAX);
    radio.setPALevel(RF24_PA_LOW);

    // Open a writing and reading pipe
    radio.openWritingPipe((byte *) TXNAME);
    radio.openReadingPipe(1, (byte *) RXNAME);

    // Start the radio listening for data
    radio.startListening();
}

void setup() {
    Serial.begin(115200);

    RGB_sensor.init();

    setupRF24();
}

uint8_t send(){
    uint16_t red   = RGB_sensor.readRed();
    uint16_t green = RGB_sensor.readGreen();
    uint16_t blue  = RGB_sensor.readBlue();

    // Write data
    snprintf((char*)data, PAYLOADLEN, "%d %d %d", red, green, blue);

    // Debug
    Serial.print(" data: ");
    Serial.println((const char*)data);

    // Stop listening so we can talk
    radio.stopListening();

    // Send data. This is a blocking call.
    uint8_t result = radio.write(data, PAYLOADLEN);

    // Resume listening
    radio.startListening();

    // Return the error code. If result is 0, there was a sending failure.
    return (result == 0); 
}

uint8_t time_to_send(){
    static uint32_t last_time = 0;
    const uint32_t delay = 200; // the period
    if(millis() - last_time > delay){
        last_time = millis();
        return true;
    }
    return false;
}

void verbose(uint8_t (*fp)(void)){
    Serial.print("Sending...");
    uint8_t result = fp();
    if(result){
        Serial.println("failure :(");
    }
    else{
        Serial.println("success!");
    }
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


// Matt: the main function that Arduino appends (I think)

int main(){
    init();
    setup();

    while(1){
        if(time_to_send()) verbose(send);
        if(received()) showdata();
    }
    return 0;
}