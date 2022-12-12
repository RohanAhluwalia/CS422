#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
    printf("pong started.\n");
    int* x = 0xefffffd0;
    *x = 1000;

    while(1){
        printf("Pong Says: %d\n", *x);
        for(int i = 0; i < 100000000; i++) {

        }
    }

    return 0;
}
