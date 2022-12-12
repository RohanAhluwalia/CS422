#include <stdlib.h> 
#include <spinlock.h>
#include <syscall.h>

#define FUTEX_WAIT 1
#define FUTEX_WAKE 2
#define FUTEX_RENQ 3

typedef struct atomic_int{
    int value;
    spinlock_t lock;
}atomic;

// void atomic_xchg(atomic *atm, int new_value){
//     spinlock_acquire(atm->lock);
//     atm->value = new_value;
//     spinlock_release(atm->lock);
// }


typedef struct _mutex {
    int ftx;
}mutex;


enum {UNLOCKED, LOCKED, CONTESTED};

void mutex_init(mutex *mtx){
    mtx->ftx = UNLOCKED;
}

int mutex_trylock(mutex *mtx){
    return mtx->ftx == UNLOCKED;
}

void mutex_lock(mutex *mtx){
    if(mutex_trylock(mtx)){
        return;
    }
    while(mtx->ftx != UNLOCKED){
        sys_futex(&mtx->ftx, FUTEX_WAIT, CONTESTED, 0, NULL);
    }
}

void mutex_unlock(mutex *mtx){
    if(mtx->ftx != CONTESTED){
        sys_futex(&mtx->ftx, FUTEX_WAIT, LOCKED, 0, NULL);
    }
}

