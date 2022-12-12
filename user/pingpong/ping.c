#include <proc.h>
#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>

#define alloca(size) __builtin_alloca(size)

int NUM_PONGS = 1;
int* PING_START_SYNC_ADDR = 0xefffffc0;
int* PONG_START_SYNC_ADDR = 0xefffffc1;

int main(int argc, char **argv)
{
    printf("Ping started prepping the Futex Demo.\n");

    // We initially create a barrier at the PROC_START_SYNC_ADDR to force all processes to wait for us to memmap relevant futex blocks.
    *PING_START_SYNC_ADDR = 0;
    *PONG_START_SYNC_ADDR = 0;
    pid_t pong_pid;
    if ((pong_pid = spawn(2, 1000)) != -1)
        printf("Launching a Pong Instance! (in process %d).\n", pong_pid);
    else
        printf("Failed to launch pong.\n");

    sys_memshare(PONG_START_SYNC_ADDR, pong_pid);
    sys_memshare(PONG_START_SYNC_ADDR, pong_pid);

    printf("Waking up other processes to start the Futex Demo.\n");
    *PONG_START_SYNC_ADDR = 1;

        



    return 0;
}
