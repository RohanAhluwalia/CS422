#include <lib/cv.h>
#include <lib/debug.h>
#include <pcpu/PCPUIntro/export.h>
#include <lib/multi_qlock.h>

#include "bbq.h"

bbq_t shared_bbq;

void bbq_init(bbq_t *bbq)
{
    bbq->front = 0;
    bbq->next_empty = 0;
    multi_qlock_init((bbq->bbq_lock));
}

void bbq_insert(bbq_t *bbq, unsigned int item)
{
    multi_qlock_aquire(&(bbq->bbq_lock));

    while ((bbq->next_empty - bbq->front) == BBQ_SIZE)
    {
        // KERN_DEBUG("\tCPU %d: Process %d: Waiting to insert %d (full buffer) at %d (front), since next_empty is %d\n", get_pcpu_idx(), get_curid(), item, bbq->front % BBQ_SIZE, bbq->next_empty % BBQ_SIZE);
        cv_wait(&(bbq->item_removed), &(bbq->bbq_lock));
        // KERN_DEBUG("\tCPU %d: Process %d: woke up from waiting to insert %d at the front (%d). next_empty is now %d\n", get_pcpu_idx(), get_curid(), item, bbq->front % BBQ_SIZE, bbq->next_empty % BBQ_SIZE);
    }
    bbq->queue[bbq->next_empty % BBQ_SIZE] = item;
    bbq->next_empty++;

    // if ((bbq->next_empty - bbq->front) == BBQ_SIZE)
    // {
    //     KERN_DEBUG("\tCPU %d: Process %d: is about to fill the buffer\n", get_pcpu_idx(), get_curid());
    // }

    cv_signal(&(bbq->item_added));
    multiq_lock_release(&(bbq->bbq_lock));
}

unsigned int bbq_remove(bbq_t *bbq)
{
    unsigned int item;

    multi_qlock_aquire(&(bbq->bbq_lock));
    while (bbq->front == bbq->next_empty)
    {
        // KERN_DEBUG("\tCPU %d: Process %d: Waiting to remove (empty buffer) from %d (front). next_empty is %d\n", get_pcpu_idx(), get_curid(), bbq->front % BBQ_SIZE, bbq->next_empty % BBQ_SIZE);
        cv_wait(&(bbq->item_added), &(bbq->bbq_lock));
        // KERN_DEBUG("\tCPU %d: Process %d: woken up from waiting on empty buffer. will now take from %d (front), and next_empty is %d\n", get_pcpu_idx(), get_curid(), bbq->front, bbq->next_empty);
    }

    item = bbq->queue[bbq->front % BBQ_SIZE];
    bbq->front++;
    if (bbq->front >= BBQ_SIZE)
    {
        bbq->front -= BBQ_SIZE;
        bbq->next_empty -= BBQ_SIZE;
    }

    cv_signal(&(bbq->item_removed));
    multiq_lock_release(&(bbq->bbq_lock));

    return item;
}