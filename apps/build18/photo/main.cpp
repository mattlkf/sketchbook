#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);

#define STEP_X 3
#define DIR_X 6
#define STEP_Y 2
#define DIR_Y 5
#define ENABLE 8
#define LASER 11

struct pkt{
    uint16_t width;
    uint16_t height;  
};

void setup(){
    Serial.begin(57600);
}

void start(){
    mySerial.begin(9600);
    mySerial.println("Hi");

}

uint8_t readPkt(char * a, uint32_t timeout){
    uint8_t i;
    uint32_t time_begin = millis();
    for(i=0;i<sizeof(struct pkt);i++){
        while(!Serial.available()){
            // wait until next character received
            if(millis() - time_begin > timeout) return 1;
        } 
        a[i] = Serial.read();
    }
    return 0;
}


void request(struct pkt * img){
    Serial.write('r');
    while(readPkt((char*) img, 1000)){
        Serial.write('r');
    }

    mySerial.print("Img: ");
    mySerial.print(img->width);
    mySerial.print(" ");
    mySerial.println(img->height);
}

int main(){
    init();
    setup();
    while(1){
        pkt img;
        start();
        request(&img);
        picture();
    }

    return 0;
}
