#include <semaphore.h>
#include <types.h>

#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))

#define FUTEX_WAIT 1
#define FUTEX_WAKE 2
#define FUTEX_RENQ 3

enum {
    LOCKED = 0,
    CONTESTED = -1
};

static inline uint32_t xchg(volatile uint32_t *addr, uint32_t newval)
{
    uint32_t result;

    asm volatile ("lock; xchgl %0, %1"
                  : "+m" (*addr), "=a" (result)
                  : "1" (newval)
                  : "cc");
    return result;
}

void sema_init(semaphore_t *lk, semaphore_t *waiters){
    *lk = 0;
    *waiters = 0;
}

int sema_trywait(semaphore_t *lk){
    int val = *(lk);

    do {
        if (val <= LOCKED){
            return 0;
        }
    } while ((val = cmpxchg(lk, val, val-1)) != val);

    return 1;
}

void sema_wait(semaphore_t *lk, semaphore_t *waiters){
    if (sema_trywait(lk)){
        return;
    }

    xchg(waiters, (*waiters)+1);
    do{
        cmpxchg(lk, LOCKED, CONTESTED);
        sys_futex((int*)lk, FUTEX_WAIT, CONTESTED, 0, NULL);
    }while(!sema_trywait(lk));

    xchg(waiters, (*waiters)-1);
}

void sema_post(semaphore_t *lk, semaphore_t *watiers, int n){
    int new, waiters, val = *lk;

    do {
        waiters = *waiters;
        new = (int) ((unsigned int)val + n + (val < LOCKED));
    } while(!cmpxchg(lk, &val, new));

    if(val < LOCKED || watiers){
        sys_futex((int*)lk, FUTEX_WAIT, n, 0, NULL);
    }
}