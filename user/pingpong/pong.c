#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
    printf("pong started.\n");
    while(1){
        int* x = 0xefffffd0;
        printf("%d\n", *x);
    }

    return 0;
}
