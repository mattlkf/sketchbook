#include <stdio.h>
#include <signal.h>
#include <unistd.h>

FILE * pwrite;

void sig_handler(int signo)
{
    fclose(pwrite);
    if (signo == SIGINT)
        printf("received SIGINT\n");
}


int main(){
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
  
    pwrite = fopen("/dev/rfcomm0", "w");

    if(pwrite == NULL){
        printf("couldn't open\n");
        return 0;
    }

    while(1){
        for(int i = 0; i < 1000; i++){
            fputc('u', pwrite);
            fflush(pwrite);
            printf("Up %d\n", i);
        }

        for(int i = 0; i < 1000; i++){
            fputc('d', pwrite);
            fflush(pwrite);
            printf("Down %d\n", i);
        }
    }
    return 0;
}