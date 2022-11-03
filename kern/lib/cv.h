#ifndef _KERN_LIB_CV_H_
#define _KERN_LIB_CV_H_

#ifdef _KERN_

#include <lib/gcc.h>
#include <lib/types.h>
#include <lib/x86.h>
#include <lib/mqueue.h>
#include "scheduler.h"

typedef struct{
    mqueue waiting;
}cv_t;

void cv_init(cv_t *cvar);
void cv_signal(cv_t *cvar);
void cv_broadcast(cv_t *cvar);
void cv_wait(cv_t *cvar, spinlock_t *lock);

unsigned int get_curid(void);
void set_curid(unsigned int curid);
void kctx_switch(unsigned int from_pid, unsigned int to_pid);
void tcb_set_state(unsigned int pid, unsigned int state);
void tqueue_enqueue(unsigned int chid, unsigned int pid);
unsigned int tqueue_dequeue(unsigned int chid);
void tqueue_remove(unsigned int chid, unsigned int pid);

#endif /* _KERN_ */

#endif /* !_KERN_LIB_CV_H_ */