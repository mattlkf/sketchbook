#include <stdio.h>

int main(){
    FILE * pwrite = fopen("/dev/rfcomm0", "w");

    if(pwrite == NULL){
        printf("couldn't open\n");
        return 0;
    }

    while(1){
        char ch;
        scanf(" %c", &ch);
        printf("Read: %c\n", ch);
       // fprintf(pwrite, "%c", ch);
        fputc(ch, pwrite);
        fflush(pwrite);
    }
    return 0;
}