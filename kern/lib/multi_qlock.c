#include <lib/debug.h>
#include <lib/x86.h>
#include <lib/spinlock.h>
#include <lib/thread.h>
#include <lib/scheduler.h>
#include <dev/intr.h>
#include <pcpu/PCPUIntro/export.h>
#include <thread/PThread/export.h>
#include <lib/seg.h>
#include <lib/trap.h>
#include "multi_qlock.h"

void multi_qlock_init(multiq_lock_t *lock){
    lock->value = FREE;
    spinlock_init(&(lock->lock));
}

void multi_qlock_aquire(multiq_lock_t *lock){
    
    intr_local_disable();
    spinlock_acquire(&(lock->lock));

    if (lock->value == BUSY){
        mqueue_enqueue(&(lock->waiting), get_curid(), get_pcpu_idx());
        scheduler_suspend_spinlock(&scheduler, &(lock->lock));
    }
    else{
        lock->value = BUSY;
        spinlock_release(&(lock->lock));
    }
    intr_local_enable();
}


void multiq_lock_release(multiq_lock_t *lock)
{
    intr_local_disable();
    spinlock_acquire(&(lock->lock));
    if (!mqueue_isempty(&(lock->waiting))){
        entry *ent = mqueue_dequeue(&(lock->waiting));
        scheduler_make_ready(&scheduler, ent->pid, ent->pcpu_idx);
    }
    else{
        lock->value = FREE;
    }
    spinlock_release(&(lock->lock));
    intr_local_enable();
}