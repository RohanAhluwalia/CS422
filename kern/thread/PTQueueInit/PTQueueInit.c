#include "lib/x86.h"

#include "import.h"

/**
 * Initializes all the thread queues with tqueue_init_at_id.
 */
void tqueue_init(unsigned int mbi_addr)
{
    // TODO: define your local variables here.

    tcb_init(mbi_addr);
    
    for(unsigned int pid = 0; pid <= NUM_IDS; pid++) {
        tqueue_init_at_id(pid);
    }
}

/**
 * Insert the TCB #pid into the tail of the thread queue #chid.
 * Recall that the doubly linked list is index based.
 * So you only need to insert the index.
 * Hint: there are multiple cases in this function.
 */
void tqueue_enqueue(unsigned int chid, unsigned int pid)
{
    // Grab the queue head and tail. 
    unsigned int head = tqueue_get_head(chid);
    unsigned int tail = tqueue_get_tail(chid);

    // Case 1: We have a blank/uninitialized queue
    if (tail == NUM_IDS) {
        tqueue_set_head(chid, pid);
        tqueue_set_tail(chid, pid);
    }
    // Case 2: We have a valid tail in the queue; we must reorder indices.
    else {
        // Set pointers for the node we're inserting
        tcb_set_prev(pid, tail);
        tcb_set_next(pid, NUM_IDS);

        // Set pointers for the old tail node.
        tcb_set_next(tail, pid);

        // Update the queue structure.
        tqueue_set_tail(chid, pid);
    }
}

/**
 * Reverse action of tqueue_enqueue, i.e. pops a TCB from the head of the specified queue.
 * It returns the popped thread's id, or NUM_IDS if the queue is empty.
 * Hint: there are multiple cases in this function.
 */
unsigned int tqueue_dequeue(unsigned int chid)
{
    // Grab the queue head and tail. 
    unsigned int head = tqueue_get_head(chid);
    unsigned int tail = tqueue_get_tail(chid);

    // Case 1: We have a blank/uninitialized queue.
    if (head == NUM_IDS) {
        return NUM_IDS;
    }
    // Case 2: We have a valid head in the queue; we must reorder indices.
    else  {

        // Obtain the new head from the old head's next. 
        unsigned int new_head = tcb_get_next(head);

        // Remove pointers for the old head node.
        tcb_set_next(head, NUM_IDS);
        tcb_set_prev(head, NUM_IDS);

        // Update the queue head.
        tqueue_set_head(chid, new_head);

        // If this was a one element queue, we've removed the only element. Set the tail to null.
        if(head == tail) {
            tqueue_set_tail(chid, NUM_IDS);
        }

        return head;
    }    
}

/**
 * Removes the TCB #pid from the queue #chid.
 * Hint: there are many cases in this function.
 */
void tqueue_remove(unsigned int chid, unsigned int pid)
{

    // Grab the queue head and tail. 
    unsigned int head = tqueue_get_head(chid);
    unsigned int tail = tqueue_get_tail(chid);


    // Case 1: The pid is at the beginning or the end. This handles the no PID case or 
    // 1 pid case as well (gets funneled through pid==head)

    if(pid == head) {
        tqueue_dequeue(chid);
    }
    else if(pid == tail) {
        // Get new tail
        unsigned int new_tail = tcb_get_prev(pid);

        // Nullify old tail.
        tcb_set_prev(pid, NUM_IDS);
        tcb_set_next(pid, NUM_IDS);

        // Set new tail next to null.
        tcb_set_next(new_tail, NUM_IDS);

        // Modify tqueue metadata
        tqueue_set_tail(chid, new_tail);
        
    }
    // Case 2: The pid is somewhere in the middle.
    else {

        // Linear scan for the correct node.
        unsigned int curr = head;
        while(curr != pid && curr != NUM_IDS) {
            curr = tcb_get_next(curr);
        }

        // If we can't find it, return.
        if(curr == NUM_IDS) {
            return;
        }

        // Get nodes TCB was sandwiched between.
        unsigned int prev = tcb_get_prev(curr);
        unsigned int next = tcb_get_next(curr);

        // Nullify curr.
        tcb_set_prev(curr, NUM_IDS);
        tcb_set_next(curr, NUM_IDS);

        // Link prev and next together.
        tcb_set_next(prev, next);
        tcb_set_prev(next, prev);

    }

}
