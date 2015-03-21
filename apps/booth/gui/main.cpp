// To compile, first install libncurses5-dev
// sudo apt-get install libncurses5-dev
//
// Then use: gcc main.cpp -lncurses

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <ncurses.h>
#include "serial.h"
#include <string.h>

const int refresh_delay = 1000;
const int min_x = 39;
const int min_y = 10;

// Globals
int scr_y, scr_x;

void printCentre(const char * s){
    mvprintw(scr_y/2, (scr_x - strlen(s))/2, "Too small!!");            
}

int main(){

    initscr(); // Initialize the window
    noecho(); // Don't echo any keypresses
    curs_set(false); // Don't display a cursor


    while(1){
        clear(); // clear the screen
        // get updated dimensions of screen (in case of resize)
        getmaxyx(stdscr, scr_y, scr_x);
        // complain if it's too small
        if(scr_y < min_y || scr_x < min_x){
            printCentre("Too small!!");
        }

        refresh();
        usleep(refresh_delay);
    }

    return 0;
}