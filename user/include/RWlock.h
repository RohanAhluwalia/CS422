#ifndef _USER_RWLOCK_H_
#define _USER_RWLOCK_H_

#include <types.h>
#include <syscall.h>

typedef unsigned _lock_t;
typedef unsigned _lock_open_t;
typedef unsigned _lock_wlocked_t;

static void _wlock();
static void _rlock();
static void _unlock();

#endif  /* !_USER_RWLOCK_H_ */