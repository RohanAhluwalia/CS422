#include <proc.h>
#include <stdio.h>
#include <syscall.h>
#include <x86.h>

int main(int argc, char **argv)
{
    printf("idle\n");
    #ifdef MUTEX_DEMO
        pid_t mutex_demo;
        if ((mutex_demo = spawn(3, 1000)) != -1){
            printf("mutex demo");
        }
    #else
        pid_t ping_pid, pong_pid;

        if ((ping_pid = spawn(1, 2000)) != -1)
            printf("ping in process %d.\n", ping_pid);
        else
            printf("Failed to launch ping.\n");
    #endif

    return 0;
}
