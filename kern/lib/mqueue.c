#include <lib/debug.h>
#include <lib/x86.h>
#include "mqueue.h"


void mqueue_init(mqueue *q){
    q->head = 0;
    q->tail = 0;
}

void mqueue_enqueue(mqueue *q, unsigned int pid, unsigned int cpu_idx){
    
    // check if queue is full
    if (q->tail - q->head > QUEUE_SIZE){
        return;
    }

    q->queue[q->tail % QUEUE_SIZE] = (entry){pid, cpu_idx};
    q->tail++;
}

entry* mqueue_dequeue(mqueue *q){

    if (mqueue_isempty(q)){
        return NULL;
    }

    entry *ent = &(q->queue[q->head % QUEUE_SIZE]);
    q->head++;

    // if we need to shift the pointers
    if(q->head >= QUEUE_SIZE){
        q->head -= QUEUE_SIZE;
        q->tail -= QUEUE_SIZE;
    }

    return ent;
}

bool mqueue_isempty(mqueue *q){
    return (q->head % QUEUE_SIZE) == (q->tail % QUEUE_SIZE);
}