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

    pid_t child = spawn(2, 1000);

    printf("child started.\n");

    // int err = sys_memshare(addr, child);

    return 0;
}
