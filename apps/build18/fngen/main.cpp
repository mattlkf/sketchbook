// A simple square-wave generator, 0.5Hz to 10KHz
// Crude in the sense that I ignore the finite time taken to compute
// That will throw off timing.

#include <Arduino.h>

#define KNOB_A A1
#define KNOB_B A2
#define LEDPIN 13

double log_1024;

void setup(){
    log_1024 = log10(1024);

    Serial.begin(115200);
    Serial.println("Ready");

    pinMode(LEDPIN, OUTPUT);
}

void myDelayMicroseconds(uint32_t t){
    while(t > 10000){
        delayMicroseconds(10000);
        t -= 10000;
    }
    delayMicroseconds(t);
}

int main(){
    init();

    setup();

    while(1){
        uint32_t a = analogRead(KNOB_A);
        uint32_t b = analogRead(KNOB_B);

        // Serial.print(a);
        // Serial.print(' ');
        // Serial.println(b);

        double period;
        
        if(b == 1023){ 
            period = 100;
        }
        else if(b == 0){ 
            period = 500000;
        }
        else{
            //everything else
            // Serial.print(b);
            // Serial.print(' ');
            // Serial.print(log10(b+1));            
            // Serial.print(' ');

            period = 0.5 + (9999.5 * log10(b+1)) / log_1024;
            period = (double) 1000000/period;
        }

        // Serial.println(1000000/period);

        b = (uint32_t) period;
        // delay
        a = (b * a) / 1023;
        if(a) digitalWrite(LEDPIN, HIGH);

        myDelayMicroseconds(a);

        digitalWrite(LEDPIN, LOW);
        myDelayMicroseconds(b-a);
    }

    return 0;
}