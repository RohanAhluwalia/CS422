#include <lib/cv.h>
#include <lib/debug.h>
#include <pcpu/PCPUIntro/export.h>

#include "bbq.h"

bbq_t shared_bbq;

void bbq_init(bbq_t *bbq)
{
    bbq->front = 0;
    bbq->next_empty = 0;
    spinlock_init(&(bbq->bbq_lock));
}

void bbq_insert(bbq_t *bbq, unsigned int item)
{
    spinlock_acquire(&(bbq->bbq_lock));
    while ((bbq->next_empty - bbq->front) == BBQ_SIZE){
        cv_wait(&(bbq->item_removed), &(bbq->bbq_lock));
    }
    bbq->queue[bbq->next_empty % BBQ_SIZE] = item;
    bbq->next_empty++;

    cv_signal(&(bbq->item_added));
    spinlock_release(&(bbq->bbq_lock));
}

unsigned int bbq_remove(bbq_t *bbq)
{
    unsigned int rmv;

    spinlock_acquire(&(bbq->bbq_lock));
    while (bbq->front == bbq->next_empty){
        cv_wait(&(bbq->item_added), &(bbq->bbq_lock));
    }

    rmv = bbq->queue[bbq->front % BBQ_SIZE];
    bbq->front++;
    if (bbq->front >= BBQ_SIZE){
        bbq->front -= BBQ_SIZE;
        bbq->next_empty -= BBQ_SIZE;
    }

    cv_signal(&(bbq->item_removed));
    spinlock_release(&(bbq->bbq_lock));

    return rmv;
}