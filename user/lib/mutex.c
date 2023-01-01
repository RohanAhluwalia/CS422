#include <types.h>
#include <mutex.h>

#define FUTEX_WAIT 1
#define FUTEX_WAKE 2
#define FUTEX_RENQ 3

enum {UNLOCKED, LOCKED, CONTESTED};

#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))

static inline uint32_t xchg(volatile uint32_t *addr, uint32_t newval)
{
    uint32_t result;

    asm volatile ("lock; xchgl %0, %1"
                  : "+m" (*addr), "=a" (result)
                  : "1" (newval)
                  : "cc");
    return result;
}

void mutex_init(mutex_t *lk){
    *lk = 0;
}

void mutex_lock(mutex_t *lk){
    int c = cmpxchg(&lk, 0 , 1);
    
    if (c != 0){
        do {
            if (c == 2 || cmpxchg(&lk,1,2) != 0){
                sys_futex((int*)lk, FUTEX_WAIT, 2, 0, NULL);
            }
        } while((c == cmpxchg(&lk, 0 ,2)) != 0);
    }
}

void mutex_unlock(mutex_t *lk){
    if( *lk != 1){
        xchg(lk, 0);
        sys_futex((int*)&lk, FUTEX_WAKE, 1, 0, NULL);
    }
}
