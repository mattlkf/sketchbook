#include <SPI.h>
#include "RF24.h"
#include "packet.h"
/**********************************************************/

#define RXNAME "tinkl"
#define TXNAME "tinkl"
#define MAX_PAYLOAD_LEN 32

RF24 radio(8,7); //(ce,csn)

/**********************************************************/

#define SAMPLE_PERIOD 1000 // milliseconds

/**********************************************************/

void setup() {
    Serial.begin(115200);

    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.openWritingPipe((byte *) TXNAME);
    radio.openReadingPipe(1, (byte *) RXNAME);
    radio.startListening();
}

tinkl_packet* make_packet(){
    static uint32_t packet_id = 0;
    static tinkl_packet pkt;
    pkt.temp = 25;
    pkt.r = pkt.g = pkt.b = 0;
    pkt.turbidity = 0;
    pkt.node_id = 150;
    pkt.packet_id = packet_id++;
    pkt.battery_voltage = 6;
    pkt.checksum = 0;
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

int main(){
    init();
    setup();

    while(1){
        if(time_to_send()){
            tinkl_packet *pp = make_packet();

            uint8_t result = send((uint8_t*) pp, sizeof(tinkl_packet));

            if(result){
                Serial.println("failure :(");
            }
            else{
                Serial.println("success!");
            }
        }

    }
    return 0;
}