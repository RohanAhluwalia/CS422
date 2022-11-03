#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
    printf("pong started.\n");

    unsigned int i;
    for (i = 0; i < 200; i++) {
        consume();
    }


    printf("pong finished\n");

    return 0;
}
