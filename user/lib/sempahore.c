#include <atomic.h>

#define FUTEX_WAIT 1
#define FUTEX_WAKE 2
#define FUTEX_RENQ 3

typedef struct sema {
    atomic ftx;
    atomic waiters;
}semaphore;

enum {
    LOCKED = 0,
    CONTESTED = -1
};

void sema_init(semaphore *s){
    atomic_xchg(s->ftx, LOCKED)
    s->waiters = 0
}

void sema_trywait(semaphore *s){
    int val = atomic_load(s->ftx);

    do {
        if (val <= LOCKED){
            return false;
        }
    } while ((val = atomic_cmpxchg(s->ftx, val, val-1)) != val);

    return true
}

void sema_wait(semaphore *s){
    if (sema_trywait(s)){
        return;
    }

    atomic_add(s->waiters);
    do{
        atomic_cmpxchg(&s->ftx, LOCKED, CONTESTED);
        sys_futex(&s->ftx, FUTEX_WAIT, CONTESTED, 0, NULL);
    }while(!sema_trywait(s));

    atomic_sub(sem->waiters)
}