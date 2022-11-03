#include <proc.h>
#include <stdio.h>
#include <syscall.h>

int main(int argc, char **argv)
{
    unsigned int i;
    printf("ping started.\n");

    // slow producing
    for (i = 0; i < 200; i++)
    {
        produce(i);
    }
    printf("ping finished\n");

    return 0;
}
