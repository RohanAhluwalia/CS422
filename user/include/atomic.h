#ifndef _USER_ATOMIC_H_
#ifndef _USER_ATOMIC_H_

#include <stdlib.h> 
#include <syscall.h>
#include "spinlock.h"

typedef struct _atomic{
    int ftx;
    spinlock_t lock;
}atomic;

void atomic_add(atomic *atm);
void atomic_sub(atomic *atm);

void atomic_xchg(atomic *atm, int new_value);
int atomic_cmpxchg(atomic *atm, int old_value, int new_value);
int atomic_load(atomic *atm);

#endif  /* !_USER_ATOMIC_H_ */
