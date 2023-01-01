#include <spinlock.h>
#include <types.h>
#include <syscall.h>
static inline uint32_t xchg(volatile uint32_t *addr, uint32_t newval)
{
    uint32_t result;

    asm volatile ("lock; xchgl %0, %1"
                  : "+m" (*addr), "=a" (result)
                  : "1" (newval)
                  : "cc");
    return result;
}

void spinlock_init(spinlock_t *lk)
{
    *lk = 0;
}

void spinlock_acquire(spinlock_t *lk)
{
    while (xchg(lk, 1) != 0)
        asm volatile ("pause");
}

// Release the lock.
void spinlock_release(spinlock_t *lk)
{
    if (spinlock_holding(lk) == FALSE)
        return;

    xchg(lk, 0);
}

// Check whether this cpu is holding the lock.
bool spinlock_holding(spinlock_t *lk)
{
    return *lk;
}

void conditioned_wait(int* location, int termination_value) {
    while(*location != termination_value) {
        sys_yield();
    }
}

void naive_wait(unsigned int time) {
    int j = 0;
    for(int i = 0; i < time; i++) {
        j ++;
    }
}

void naive_memshare_coord_set(int* addr, int desired_val, unsigned int times_to_try) {
    for(unsigned int i = 0; i < times_to_try; i++) {
        *addr = desired_val;
    }
}

/* 
For any memmapped process, we want to set the initial value of the memory so that 
memap preconditions are met. Then, we want to yield to another process (i.e. the parent) 
to perform the mapping.
*/
void startup_memmapped_process(int* shared_location, int val) {
    *shared_location = val;
    sys_yield();
}

void memshare_set_and_yield(int* location, int val) {
    *location = val; 
    sys_yield();
}