// To compile, first install libncurses5-dev
// sudo apt-get install libncurses5-dev
//
// Then use: gcc main.cpp -lncurses

#include <stdio.h>
#include <ncurses.h>

int main()
{   

    FILE * pwrite = fopen("/dev/rfcomm0", "w");

    if(pwrite == NULL){
        printf("couldn't open rfcomm\n");
        return 0;
    }


    int c;

    initscr();
    clear();
    noecho();
    cbreak();   /* Line buffering disabled. pass on everything */
    
    keypad(stdscr, TRUE);
    mvprintw(0, 0, "Use arrow keys to choose direction of motion");
    mvprintw(1, 0, "Press the same key to undo your selection");
    mvprintw(2, 0, "Spacebar switches between high/low speed");
    refresh();
    while(1)
    {   c = getch();
        // clear();
        switch(c)
        {   
            case ' ':
                fputc(' ', pwrite);
                fflush(pwrite);

                mvprintw(5, 0, "SPEED");
                clrtoeol();
                break;

            case KEY_UP:
                fputc('u', pwrite);
                fflush(pwrite);

                mvprintw(5, 0, "UP");
                clrtoeol();
                break;
            case KEY_DOWN:
                fputc('d', pwrite);
                fflush(pwrite);

                mvprintw(5, 0, "DOWN");
                clrtoeol();
                break;
            case KEY_LEFT:
                fputc('r', pwrite);
                fflush(pwrite);

                mvprintw(5, 0, "LEFT");
                clrtoeol();
                break;
            case KEY_RIGHT:
                fputc('l', pwrite);
                fflush(pwrite);

                mvprintw(5, 0, "RIGHT");
                clrtoeol();
                break;
            default:
                mvprintw(5, 0, "Undefined: %3d '%c'", c, c);
                clrtoeol();
                break;
        }

        refresh();
    }   
    clrtoeol();
    refresh();
    endwin();
    return 0;
}
