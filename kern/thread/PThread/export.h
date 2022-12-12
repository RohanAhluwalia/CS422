#ifndef _KERN_THREAD_PTHREAD_H_
#define _KERN_THREAD_PTHREAD_H_

#ifdef _KERN_

#include <kern/lib/spinlock.h>

void thread_init(unsigned int mbi_addr);
unsigned int thread_spawn(void *entry, unsigned int id,
                          unsigned int quota);
void thread_yield(void);
void sched_update(void);
void thread_sleep(void *chan, spinlock_t *lk);
void thread_wakeup(void *chan);
void thread_wakeup_limited(void* chan, unsigned int max_threads);
void thread_requeue_limited(void* chan, unsigned int number_of_threads_to_wake, void* new_chan);

#endif  /* _KERN_ */

#endif  /* !_KERN_THREAD_PTHREAD_H_ */
