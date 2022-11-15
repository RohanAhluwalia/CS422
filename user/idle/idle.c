#include <proc.h>
#include <stdio.h>
#include <syscall.h>
#include <x86.h>

int main(int argc, char **argv)
{
    printf("idle\n");

    pid_t shell;
    if ((shell = spawn(5, 1000)) != -1){
        printf("shell has started \n");
    }else{
        printf("ERROR: shell failed to start");
    }

    // pid_t fstest_pid;

    // if ((fstest_pid = spawn(4, 1000)) != -1)
    //     printf("fstest in process %d.\n", fstest_pid);
    // else
    //     printf("Failed to launch fstest.\n");

    return 0;
}
