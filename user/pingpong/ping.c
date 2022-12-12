#include <proc.h>
#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>

#define alloca(size) __builtin_alloca(size)


int main(int argc, char **argv)
{
    printf("ping started.\n");

    // int *address = alloca(sizeof(int)); 
    // printf("%p",address);

    int *addr = 0xefffffd0;
    *addr = 999;

    pid_t pong_pid;

    if ((pong_pid = spawn(2, 1000)) != -1)
        printf("pong in process %d.\n", pong_pid);
    else
        printf("Failed to launch pong.\n");

    int err = sys_memshare(addr, pong_pid);

    return 0;
}
