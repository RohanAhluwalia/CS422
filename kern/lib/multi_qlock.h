#ifndef _KERN_LIB_MULTI_QLOCK_H_
#define _KERN_LIB_MULTI_QLOCK_H_

#ifdef _KERN_

#include <lib/gcc.h>
#include <lib/types.h>
#include <lib/spinlock.h>
#include "mqueue.h"

#define FREE 0
#define BUSY 1

typedef struct {
    unsigned int value;
    spinlock_t lock;
    mqueue waiting;
}multiq_lock_t;

void multiq_lock_init(multiq_lock_t *lock);
void multiq_lock_aquire(multiq_lock_t *lock);
void multiq_lock_release(multiq_lock_t *lock);


//addtionally needed function


#endif /* _KERN_ */

#endif /* !_KERN_LIB_MULTI_QLOCK_H_ */