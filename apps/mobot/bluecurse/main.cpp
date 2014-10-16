// To compile, first install libncurses5-dev
// sudo apt-get install libncurses5-dev

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
    mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
    refresh();
    while(1)
    {   c = getch();
        // clear();
        switch(c)
        {   
            case ' ':
                fputc(' ', pwrite);
                fflush(pwrite);

                mvprintw(24, 0, "SPEED");
                clrtoeol();
                break;

            case KEY_UP:
                fputc('u', pwrite);
                fflush(pwrite);

                mvprintw(24, 0, "UP");
                clrtoeol();
                break;
            case KEY_DOWN:
                fputc('d', pwrite);
                fflush(pwrite);

                mvprintw(24, 0, "DOWN");
                clrtoeol();
                break;
            case KEY_LEFT:
                fputc('l', pwrite);
                fflush(pwrite);

                mvprintw(24, 0, "LEFT");
                clrtoeol();
                break;
            case KEY_RIGHT:
                fputc('r', pwrite);
                fflush(pwrite);

                mvprintw(24, 0, "RIGHT");
                clrtoeol();
                break;
            default:
                mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
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
