#include <proc.h>
#include <stdio.h>
#include <syscall.h>
#include <mutex.h>


int* MEMSHARE_SYNC_ADDR = 0xefffffc3;
mutex_t *LOCK = 0xefffffc2;



int main(int argc, char **argv)
{
    startup_memmapped_process(MEMSHARE_SYNC_ADDR, 0);
    printf("[Pong] Pong instance created! Alerting ping by setting shared memory to 1...\n");
    *MEMSHARE_SYNC_ADDR = 1;
    sys_yield();

    // Wait for us to be 0 again
    conditioned_wait(MEMSHARE_SYNC_ADDR, 0);
    printf("[Pong] Value at MEMSHARE_SYNC_ADDR: %d\n", *MEMSHARE_SYNC_ADDR);
    printf("[Pong] Ping alerted me it completed the initial demo by setting sync addr to 0 again. Now we can start other tests!\n");

    sys_yield();
    //printf("Pong instance ready to go! Beginning the futex demo.\n");
    while(1) {

    }
    return 0;
}
