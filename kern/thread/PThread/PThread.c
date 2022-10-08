#include <lib/x86.h>
#include <lib/thread.h>

#include "import.h"

void thread_init(unsigned int mbi_addr)
{
    tqueue_init(mbi_addr);
    set_curid(0);
    tcb_set_state(0, TSTATE_RUN);
}

/**
 * Allocates a new child thread context, sets the state of the new child thread
 * to ready, and pushes it to the ready queue.
 * It returns the child thread id.
 */
unsigned int thread_spawn(void *entry, unsigned int id, unsigned int quota)
{
    // Allocate the new child.
    unsigned int new_chid = kctx_new(entry, id, quota);
    
    // Check that we're not out of memory.
    if(new_chid == NUM_IDS) {
        return NUM_IDS;
    }

    // Set state to ready
    tcb_set_state(new_chid, TSTATE_READY);

    // Push to ready queue. This queue should be at NUM_IDS.
    tqueue_enqueue(NUM_IDS, new_chid);

    return new_chid;
}

/**
 * Yield to the next thread in the ready queue.
 * You should set the currently running thread state as ready,
 * and push it back to the ready queue.
 * Then set the state of the popped thread as running, set the
 * current thread id, and switch to the new kernel context.
 * Hint: If you are the only thread that is ready to run,
 * do you need to switch to yourself?
 */
void thread_yield(void)
{

    // Get the current thread id and next thread id.
    unsigned int curr = get_curid();
    unsigned int next = tqueue_dequeue(NUM_IDS);

    // If there's nothing on the ready queue, we don't need to perform a switch.
    if(next == NUM_IDS) {
        return;
    }

    // Set current thread state to ready and move to back of queue.
    tcb_set_state(curr, TSTATE_READY);
    tqueue_enqueue(NUM_IDS, curr);

    // Set state of next as running. Set current thread id and switch.
    tcb_set_state(next, TSTATE_RUN);
    set_curid(next);
    kctx_switch(curr, next);
}
