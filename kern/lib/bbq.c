#include <lib/cv.h>
#include <lib/debug.h>
#include <pcpu/PCPUIntro/export.h>

#include "bbq.h"

bbq_t shared_bbq;

void bbq_init(bbq_t *bbq)
{
    bbq->head = 0;
    bbq->next = 0;
    spinlock_init(&(bbq->lock));
}

void bbq_insert(bbq_t *bbq, unsigned int insert)
{
    spinlock_acquire(&(bbq->lock));
    while ((bbq->next - bbq->head) == BBQ_SIZE){
        cv_wait(&(bbq->removed), &(bbq->lock));
    }
    bbq->queue[bbq->next % BBQ_SIZE] = insert;
    bbq->next++;

    cv_signal(&(bbq->added));
    spinlock_release(&(bbq->lock));
}

unsigned int bbq_remove(bbq_t *bbq)
{
    unsigned int rmv;

    spinlock_acquire(&(bbq->lock));
    while (bbq->head == bbq->next){
        cv_wait(&(bbq->added), &(bbq->lock));
    }

    rmv = bbq->queue[bbq->head % BBQ_SIZE];
    bbq->head++;
    if (bbq->head >= BBQ_SIZE){
        bbq->next -= BBQ_SIZE;
        bbq->head -= BBQ_SIZE;
    }

    cv_signal(&(bbq->removed));
    spinlock_release(&(bbq->lock));

    return rmv;
}