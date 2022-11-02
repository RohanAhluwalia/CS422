#ifndef _KERN_LIB_MQUEUE_H_
#define _KERN_LIB_MQUEUE_H_

#ifdef _KERN_

/**
 * Mulithreading queue implemented with a max size of NUM_IDS
 * 
*/
#define QUEUE_SIZE NUM_IDS

typedef struct {
    unsigned int pid;
    unsigned int pcpu_idx;
}entry;

typedef struct {
    unsigned int head;
    unsigned int tail;
    entry queue[QUEUE_SIZE];
} mqueue;

void mqueue_init(mqueue *q);
void mqueue_enqueue(mqueue *q, unsigned int pid, unsigned int cpu_idx);
entry* mqueue_dequeue(mqueue *q);
bool mqueue_isempty(mqueue *q);

#endif /* _KERN_ */

#endif /* !_KERN_LIB_MQUEUE_H_ */