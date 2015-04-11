// To compile, first install libncurses5-dev
// sudo apt-get install libncurses5-dev
//
// Then use: gcc main.c serial.c -lncurses

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>
#include "serial.h"
#include <string.h>
#include <signal.h>
#include <stdlib.h>

// Parameters
#define nsensors 40

const int refresh_delay = 1000;
const int min_x = 78;
const int min_y = 6;

// Globals
int scr_y, scr_x;
int onstate[nsensors];

void sig_handler(int signo){
    serial_close();
    mvprintw(scr_y-1, 0, "byebye! ^_^ closing serial~~~");
    refresh();
    usleep(500000);
    endwin();
    exit(0);
}

void printCentre(const char * s){
    mvprintw(scr_y/2, (scr_x - strlen(s))/2, s);            
}

void printArray(){
    // printCentre("whee");
    int i;
    const int gap = 2;
    // First row
    for(i=0;i < nsensors;i+=2){
        mvprintw(scr_y/2 - 2, scr_x - 4 - (i * gap), "%c", onstate[i] ? 'X' : '_');
    }           

    // Second row
    for(i=0;i+1 < nsensors;i+=2){
        mvprintw(scr_y/2 + 1, scr_x - 4 - (i * gap), "%c", onstate[i+1] ? 'X' : '_');
    }           

}

int main(){
    // Catch kill signal and shut down serial connection gracefully
    signal(SIGINT, sig_handler);

    // GUI
    initscr(); // Initialize the window
    noecho(); // Don't echo any keypresses
    curs_set(false); // Don't display a cursor

    // SERIAL
    serial_init("/dev/ttyUSB0", "9600");

    while(1){
        clear(); // clear the screen
        int i;

        // Serial stuff
        serial_fill();
        if(serial_available()){
            uint8_t read = serial_nextchar();
            read -= 20;
            // mvprintw(0,0,"Read %d", read);
            if(read < nsensors * 2){
                //update the sensor state array
                onstate[read/2] = read % 2;
            }
        }
        // else{
        //     mvprintw(0,0,"Serial unavailable");            
        // }

        // handle display stuff

        // get updated dimensions of screen (in case of resize)
        getmaxyx(stdscr, scr_y, scr_x);
        // complain if it's too small
        if(scr_y < min_y || scr_x < min_x){
            printCentre("Too small!!");
        }
        else{
            printArray();
        }

        refresh();
        usleep(refresh_delay);
    }

    return 0;
}