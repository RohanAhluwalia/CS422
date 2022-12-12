#include <proc.h>
#include <stdio.h>
#include <syscall.h>



int* PING_START_SYNC_ADDR = 0xefffffc0;
int* PONG_START_SYNC_ADDR = 0xefffffc1;


int main(int argc, char **argv)
{
    
    printf("Pong instance created! Waiting on coordinator to begin the demo...\n");

    while(*PONG_START_SYNC_ADDR != 1) {
        // Spinlock wait for coordinator to set up memory sharing.
    }

    printf("Pong instance ready to go! Beginning the futex demo.\n");
    return 0;
}
