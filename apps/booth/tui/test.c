// Compile with: gcc test.c serial.c -o test


#include <stdio.h>
#include "serial.h"
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>

void sig_handler(int signo){
    serial_close();

    printf("Received SIGINT, closing serial port and exiting\n");

    // if (signo == SIGUSR1)
    //     printf("received SIGUSR1\n");
    // else if (signo == SIGKILL)
    //     printf("received SIGKILL\n");
    // else if (signo == SIGSTOP)
    //     printf("received SIGSTOP\n");
    // else if (signo == SIGINT)
    //     printf("received SIGINT\n");
    exit(1);    
}


int main(){
    // Catch kill signal and shut down serial connection gracefully
    // signal(SIGKILL, sig_handler);
    // signal(SIGTERM, sig_handler);
    // signal(SIGSTOP, sig_handler);
    signal(SIGINT, sig_handler);

    // SERIAL
    serial_init("/dev/ttyUSB0", "9600");

    int i = 0;
    while(1){
        // Serial stuff
        // printf("a %d\n", i++);
        serial_fill();
        // printf("b\n");
        if(serial_available()){
            // printf("c\n");
            uint8_t read = serial_nextchar();
            printf("Read: %u\n", read);
        }
        
    }

    return 0;
}