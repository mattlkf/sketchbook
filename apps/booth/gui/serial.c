#include "serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>

#define SERIAL_BUF_MAX 200

int fd = -1;
char buf[SERIAL_BUF_MAX + 1];
int n = 0; // number of characters in buffer
int p = 0; // pointer to current position in buffer


char * serial_read(){
    if(fd == -1) printf("Error: Port not opened\n");
    n = read(fd, buf, SERIAL_BUF_MAX);

    buf[n] = 0;
    return buf;
}

void serial_fill(){
    if(p >= n){
        serial_read();
        p = 0;
    }
}

char serial_nextchar(){
    // while(p >= n){ // while buffer is empty
    //     // try refill the buffer
    //     serial_read();
    //     p = 0;
    // }

    //drip-feed the buffer
    return buf[p++];
}

int serial_available(){
    return p < n;
}

void serial_close(){
    if(fd != -1) close(fd);
    else printf("Error: not open; can't close\n");
}

int serial_init(const char * tty_dev, const char * baud_str){
    int baud;
    sscanf(baud_str, "%d", &baud);

    struct termios toptions;

    /* open serial port */
    fd = open(tty_dev, O_RDWR | O_NOCTTY);
    if(fd == -1){
        printf("Error: Cannot open %s\n", tty_dev);
        return -1;
    }
    printf("%s opened as %i\n", tty_dev, fd);

    /* wait for the Arduino to reboot */
    // usleep(3500000);

    /* Choose the right baud rate identifier */
    speed_t br;
    switch (baud) {
    case 9600:   br = B9600;   break;
    case 19200:  br = B19200;  break;
    case 38400:  br = B38400;  break;
    case 57600:  br = B57600;  break;
    case 115200: br = B115200; break;
    default:     
        printf("Unknown baud rate\n");
        printf("Using default of 9600\n");
        br = B9600;
        break;
    }
    printf("Baud rate %d\n", baud);

    /* get current serial port settings */
    tcgetattr(fd, &toptions);
    /* set baud rate both ways */
    cfsetispeed(&toptions, br);
    cfsetospeed(&toptions, br);
    /* 8 bits, no parity, no stop bits */
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    /* Canonical mode */
    /* i.e. line buffering - chars will not come through until a \n is sent */
    // toptions.c_lflag |= ICANON;
    /* Non-canonical mode */
    toptions.c_lflag &= ~ICANON;

    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 0;

    /* commit the serial port settings */
    tcsetattr(fd, TCSANOW, &toptions);


    return 0;
}
