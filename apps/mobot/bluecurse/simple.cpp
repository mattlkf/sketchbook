#include <stdio.h>
#include <stdint.h>

int main(){
    
    FILE * pwrite = fopen("/dev/rfcomm0", "w");

    if(pwrite == NULL){
        printf("couldn't open rfcomm\n");
        return 0;
    }

    while(1){
        uint8_t x;
        scanf("%hhd", &x);
        fputc(x, pwrite);
        fflush(pwrite);
    }

    return 0;
}