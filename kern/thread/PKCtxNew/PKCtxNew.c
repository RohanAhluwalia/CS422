#include <lib/gcc.h>
#include <lib/x86.h>

#include "import.h"

extern char STACK_LOC[NUM_IDS][PAGESIZE] gcc_aligned(PAGESIZE);

/**
 * Checks whether enough resources are available for allocation and
 * allocates memory for the new child thread, then sets the eip, and esp
 * of the thread states. The eip should be set to [entry], and the
 * esp should be set to the corresponding stack TOP in STACK_LOC.
 * Don't forget the stack is going down from high address to low.
 * We do not care about the rest of states when a new thread starts.
 * The function returns the child thread (process) id.
 * In case of an error, return NUM_IDS.
 */
unsigned int kctx_new(void *entry, unsigned int id, unsigned int quota)
{
    // Check whether enough resources are available.
    if(!container_can_consume(id, quota)) {
        return NUM_IDS;
    }
    
    // Allocate resources
    unsigned int child = alloc_mem_quota(id, quota);
    if(child == NUM_IDS) {
        return NUM_IDS;
    }
    

    // Set eip and esp. 
    kctx_set_eip(child, entry);

    // QUESTION: Is the ESP top equal to PAGESIZE - 1 or Zero? 
    kctx_set_esp(child,  STACK_LOC[id] + sizeof(char)*(PAGESIZE-1));

    
    return child;
}
