#include <atomic.h>

#define FUTEX_WAIT 1
#define FUTEX_WAKE 2
#define FUTEX_RENQ 3

typedef struct _mutex {
    atomic ftx;
}mutex;


enum {UNLOCKED, LOCKED, CONTESTED};

void mutex_init(mutex *mtx){
    atomic_xchg(mtx->ftx, UNLOCKED);
}

int mutex_trylock(mutex *mtx){
    return atomic_cmpxchg(mutex->ftx, UNLOCKED, LOCKED) == UNLOCKED;
}

void mutex_lock(mutex *mtx){
    if(mutex_trylock(mtx)){
        return;
    }
    while(atomic_xchg(mtx->ftx, CONTESTED) != UNLOCKED){
        sys_futex(&mtx->ftx, FUTEX_WAIT, CONTESTED, 0, NULL);
    }
}

void mutex_unlock(mutex *mtx){
    if(atomic_xchg(mtx->ftx, UNLOCKED) != CONTESTED){
        sys_futex(&mtx->ftx, FUTEX_WAIT, LOCKED, 0, NULL);
    }
}

