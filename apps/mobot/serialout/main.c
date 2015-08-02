#include <stdio.h>
#include "serial.h"
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>

void sig_handler(int signo){
    serial_close();
    exit(1);    
}

void set_speeds(uint8_t r, uint8_t l){
    uint8_t buf[4] = "r l ";
    buf[1] = r;
    buf[3] = l;
    serial_write(buf, 4);
}

int main(){
    // Catch kill signal and shut down serial connection gracefully
    signal(SIGINT, sig_handler);

    // SERIAL
    // serial_init("/dev/ttyO1", "9600");
    serial_init("/dev/ttyUSB0", "9600");

    while(1){
        uint8_t a, b;
        scanf("%hhd %hhd", &a, &b);
        set_speeds(a,b);
    }

    return 0;

}