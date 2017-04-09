// This program reads the sensors on the puck and transmits a packet to the hub.

// This version makes no attempt to enter low-power mode

/**********************************************************/

// Radio - nRF24L01+
#include <SPI.h>
#include "RF24.h"
#include "packet.h"

// ADC - MCP3425
#include <Wire.h>
#include <MCP342X.h>

// Sensors
#include "color.h"
#include "temperature.h"
#include "turbidity.h"

/**********************************************************/

// Set NODE_ID to a default value if it was not specified with a compiler flag
#ifndef NODE_ID
    #define NODE_ID 549
#endif

/**********************************************************/

#define RXNAME "tinkl"
#define TXNAME "tinkl"
#define MAX_PAYLOAD_LEN 32

RF24 radio(8,7); //(ce,csn)

/**********************************************************/

#define SAMPLE_PERIOD 1000 // milliseconds

/**********************************************************/

MCP342X MCP_ADC;

/**********************************************************/

#define BATTERY_PIN A1

/**********************************************************/

void setup() {
    Serial.begin(115200);

    // Initialize radio
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.openWritingPipe((byte *) TXNAME);
    radio.openReadingPipe(1, (byte *) RXNAME);
    radio.startListening();

    // Initialize the (possibly shared) ADC
    Wire.begin();  // join I2C bus
    TWBR = 12;  // 400 kHz (maximum)
    Serial.println(MCP_ADC.testConnection() ? "MCP342X connection successful" : "MCP342X connection failed");
    MCP_ADC.configure(  MCP342X_MODE_CONTINUOUS |
                        MCP342X_CHANNEL_1 |
                        MCP342X_SIZE_16BIT |
                        MCP342X_GAIN_1X
                    );
    Serial.println(MCP_ADC.getConfigRegShdw(), HEX);

    // Initialize sensors
    if(init_color() == false) Serial.println("Failed to init color sensor");   
    if(init_turbidity(&MCP_ADC) == false) Serial.println("Failed to init turbidity sensor");   
    if(init_temp(&MCP_ADC) == false) Serial.println("Failed to init temperature sensor");   
}

void read_battery(uint16_t &voltage){
    voltage = analogRead(BATTERY_PIN);
}

// Todo
void write_checksum(tinkl_packet &pkt){
    pkt.checksum = 0;
}

tinkl_packet* make_packet(){
    static tinkl_packet pkt;
    
    // Identify the source with our node ID
    pkt.node_id = NODE_ID;

    // Assign a new packet ID
    static uint32_t packet_id = 0;
    pkt.packet_id = packet_id++;

    // Populate the sensor-data fields with RAW sensor readings
    // Although these are raw values, we can convert them to physical units
    // at the back-end. This lets us avoid storing calibration constants
    // on the puck.
    read_temp(pkt.temp);
    read_color(pkt.r, pkt.g, pkt.b);
    read_turbidity(pkt.turbidity);
    read_battery(pkt.battery_voltage);

    write_checksum(pkt);
    return &pkt;
}

uint8_t send(uint8_t *data, uint8_t length){
    if(length > MAX_PAYLOAD_LEN) return 1;
    radio.stopListening();
    uint8_t ret_code = radio.write(data, length);
    radio.startListening();
    return (ret_code == 0); // if ret_code is 0, return 1 to signal an error
}

uint8_t time_to_send(){
    static uint32_t last_time = 0;
    const uint32_t delay = SAMPLE_PERIOD;
    if(millis() - last_time > delay){
        last_time = millis();
        return true;
    }
    return false;
}

void show_received_data(){
    static uint8_t data[MAX_PAYLOAD_LEN];
    radio.read(data, MAX_PAYLOAD_LEN);

    Serial.print("Received: ");
    for(uint8_t i=0; i<MAX_PAYLOAD_LEN;i++){
        Serial.print(data[i]);
    }
    Serial.println();
}

int main(){
    init();
    setup();

    while(1){
        if(time_to_send()){
            // Make a packet!
            tinkl_packet *pkt = make_packet();

            // Send it!
            Serial.print("Sending... ");
            uint8_t result = send((uint8_t*) pkt, sizeof(tinkl_packet));

            // Did sending succeed?
            Serial.println(result ? "failed :(" : "success :)");
        }

        if(radio.available()){
            show_received_data();
        }

    }
    return 0;
}