#include <lib/debug.h>
#include <lib/x86.h>
#include <lib/thread.h>
#include <lib/scheduler.h>
#include <pcpu/PCPUIntro/export.h>
#include <thread/PThread/export.h>


#include "scheduler.h"
#include "cv.h"


void cv_wait(cv_t *cvar, multiq_lock_t *lock)
{
    mqueue_enqueue(&(cvar->waiting), get_curid(), get_pcpu_idx());
    scheduler_suspend_multiq_lock(&scheduler, lock);
    multi_qlock_aquire(lock);
}

void cv_signal(cv_t *cvar)
{
    if (mqueue_isempty(&(cvar->waiting))){
        return;
    }
    entry *elt = mqueue_dequeue(&(cvar->waiting));
    scheduler_make_ready(&scheduler, elt->pid, elt->pcpu_idx);
}


void cv_broadcast(cv_t *cvar)
{
    entry *elt;
    while (!mqueue_isempty(&(cvar->waiting)))
    {
        elt = mqueue_dequeue(&(cvar->waiting));
        scheduler_make_ready(&scheduler, elt->pid, elt->pcpu_idx);
    }
}


