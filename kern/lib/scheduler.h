#ifndef _KERN_LIB_SCHEDULER_H_
#define _KERN_LIB_SCHEDULER_H_

#ifdef _KERN_

#include <lib/gcc.h>
#include <lib/types.h>
#include <lib/x86.h>
#include <lib/spinlock.h>

typedef struct{
    spinlock_t spinlock;
}scheduler_t;

extern scheduler_t scheduler;

void scheduler_init(scheduler_t *scheduler);
void scheduler_make_ready(scheduler_t *scheduler, unsigned int pid, unsigned int pcpu_idx);
void scheduler_suspend_spinlock(scheduler_t *scheduler, spinlock_t *spinlock);


unsigned int get_curid(void);
void set_curid(unsigned int id);
unsigned int tqueue_dequeue(unsigned int chid);
void tqueue_remove(unsigned int child, unsigned int pid);
void kctx_switch(unsigned int src, unsigned int to);
void tcb_set_state(unsigned int pid, unsigned int state);
void tqueue_enqueue(unsigned int child, unsigned int pid);

#endif /* _KERN_ */

#endif /* !_KERN_LIB_SCHEDULER_H_ */