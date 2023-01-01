#include <proc.h>
#include <syscall.h>
#include <stdlib.h>
#include <stdio.h>
#include <mutex.h>

#define alloca(size) __builtin_alloca(size)

int NUM_PONGS = 1;
int* MEMSHARE_SYNC_ADDR = 0xefffffc3;
mutex_t *LOCK = 0xefffffc2;

int main(int argc, char **argv)
{
    startup_memmapped_process(MEMSHARE_SYNC_ADDR, 0);

    printf("[Ping] Ping started prepping the Futex Demo.\n");
    // We initially create a barrier at the MEMSHARE_SYNC_ADDR to force all processes to wait for us to memmap relevant futex blocks.
    *MEMSHARE_SYNC_ADDR = 0;

    pid_t pong_pid;
    printf("[Ping] Initial value at MEMSHARE_SYNC_ADDR: %d\n", *MEMSHARE_SYNC_ADDR);

    if ((pong_pid = spawn(2, 1000)) != -1)
        printf("[Ping] Launching a Pong Instance! (in process %d).\n", pong_pid);
    else
        printf("[Ping] Failed to launch pong.\n");

    // Memshare necessary pages for the demo.
    printf("[Ping] Memory mapping key shared addresses...\n");
    sys_memshare(MEMSHARE_SYNC_ADDR, pong_pid);
    //sys_memshare(LOCK, pong_pid);
    
    printf("[Ping] Waiting for other processes to notify ping that they're ready via changing the value at a memshared address...\n");
    // Wait for Pong to set the sync address to 1.
    conditioned_wait(MEMSHARE_SYNC_ADDR, 1);

    printf("[Ping] Value at MEMSHARE_SYNC_ADDR: %d\n", *MEMSHARE_SYNC_ADDR);
    printf("[Ping] As you can see, the value that was previously 0 in ping is now 1 despite it being set in pong and unmanipulated in ping besides initially setting it. This concludes the memshare demo.\n");
    printf("[Ping] Alerting pong and setting the sync address back to 0...\n");
    *MEMSHARE_SYNC_ADDR = 0;
    sys_yield();


    // MUTEX TEST

    int64_t v = 0;
    mutex_init(LOCK);
    printf("Lock value: %p\n", *LOCK);

    while(1) {

    }

    return 0;
}
