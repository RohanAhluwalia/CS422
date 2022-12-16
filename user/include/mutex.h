#ifndef _USER_MUTEX_H_
#define _USER_MUTEX_H_

#include <types.h>
#include <syscall.h>

typedef volatile uint32_t mutex_t;

void mutex_init(mutex_t *lk);
void mutex_lock(mutex_t *lk);
void mutex_unlock(mutex_t *lk);

#endif  /* !_USER_MUTEX_H_ */