#include <SPI.h>
#include "RF24.h"

/**********************************************************/
#define RXNAME "serv1"
#define TXNAME "serv1"
#define PAYLOADLEN 32
uint8_t data[PAYLOADLEN];

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(8,7); //(ce,csn)
/**********************************************************/

bool read_mode = true;

struct tinkl_packet{
    uint16_t temp;
    uint16_t r;
    uint16_t g;
    uint16_t b;
    uint16_t turbidity;
    uint32_t node_id;
    uint32_t packet_id;
    uint16_t battery_voltage;
    uint16_t checksum;
};

void print_packet(tinkl_packet *p){
// printf("Temp: %u\n", p->temp);
// printf("RGB: %u %u %u\n", p->r, p->g, p->b);
// printf("Turbidity: %u\n", p->turbidity);
// printf("Node: %u\n", p->node_id);
// printf("Packet: %u\n", p->packet_id);
    unsigned int i;
    for(i=0;i<sizeof(tinkl_packet);i++){
        Serial.print(((uint8_t*)p)[i]);
        Serial.print(" ");
    }
    Serial.println();
}

void setup() {
    Serial.begin(115200);

    radio.begin();
    radio.setPALevel(RF24_PA_LOW);

    // Open a writing and reading pipe
    radio.openWritingPipe((byte *) TXNAME);
    radio.openReadingPipe(1, (byte *) RXNAME);

    // Start the radio listening for data
    radio.startListening();
}

tinkl_packet* make_packet(){
    static uint32_t packet_id = 0;
    static tinkl_packet pkt;
    pkt.temp = 25;
    pkt.r = pkt.g = pkt.b = 0;
    pkt.turbidity = 0;
    pkt.node_id = 451;
    pkt.packet_id = packet_id++;
    pkt.battery_voltage = 6;
    pkt.checksum = 0;
    return &pkt;
}

uint8_t send(tinkl_packet *pkt){

    // Stop listening so we can talk
    radio.stopListening();

    // Send data. This is a blocking call.
    // uint8_t result = radio.write((uint8_t*)data, PAYLOADLEN);
    uint8_t result = radio.write((uint8_t*)pkt, sizeof(tinkl_packet));

    Serial.println(sizeof(tinkl_packet));

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

// Make sure there is actually data
    if(!radio.available()) return;

// Get the data
    radio.read(data, PAYLOADLEN);

// For debugging: dump the 32 bytes' values
    // if(read_mode){
        Serial.println("---Received---");
        for(i=0; i<PAYLOADLEN;i++){
            Serial.print(data[i]);
            Serial.write(' ');
        }
        Serial.println();

        for(i=0; i<PAYLOADLEN;i++){
            Serial.write(data[i]);
        }
        Serial.println();
        Serial.println();
    // }
}

uint8_t time_to_send(){
    static uint32_t last_time = 0;
    const uint32_t delay = 1000; // the period
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

uint8_t pressed(){
    uint8_t pflag = 0;
    while(Serial.available()){ // empty the serial buffer
        pflag = 1;
        Serial.read();
    }
    return pflag;
}

void toggle_mode(){
    read_mode = !read_mode;
}

int main(){
    init();
    setup();

    while(1){
        // if(!read_mode && time_to_send()){
        if(time_to_send()){
            Serial.print("Sending...");
            tinkl_packet *pp = make_packet();
            print_packet(pp);

            uint8_t result = send(pp);

            // Serial.println(pp->temp);
            if(result){
                Serial.println("failure :(");
            }
            else{
                Serial.println("success!");
            }
        }

        // if(received()) showdata();
        // if(pressed()) toggle_mode();
    }
    return 0;
}