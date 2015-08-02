#include <stdio.h>
#include "serial.h"
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>

void sig_handler(int signo){
    serial_close();
    exit(1);    
}



int main(){
    // Catch kill signal and shut down serial connection gracefully
    // signal(SIGKILL, sig_handler);
    // signal(SIGTERM, sig_handler);
    // signal(SIGSTOP, sig_handler);
    signal(SIGINT, sig_handler);

    // SERIAL
    // serial_init("/dev/ttyO1", "9600");
    serial_init("/dev/ttyUSB0", "9600");

    while(1){
        // Serial stuff
        // printf("a %d\n", i++);
        serial_fill();
        // printf("b\n");
        if(serial_available()){
            // printf("c\n");
            uint8_t read = serial_nextchar();
            if(read == 'v'){
                printf("Hi!\n", read);
            }
        }
        
    }

    return 0;

}