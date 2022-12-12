#include <atomic.h>

void atomic_add(atomic *atm){
    spinlock_acquire(atm->lock);
    atm->value += 1;
    spinlock_release(atm->lock);

}

void atomic_sub(atomic *atm){
    spinlock_acquire(atm->lock);
    atm->value -= 1;
    spinlock_release(atm->lock);

}

void atomic_xchg(atomic *atm, int new_value){
    spinlock_acquire(atm->lock);
    atm->value = new_value;
    spinlock_release(atm->lock);
}

int atomic_cmpxchg(atomic *atm, int old_value, int new_value){
    spinlock_acquire(atm->lock);
    if atm->value == old_value{
        atm->value = new_value
    }
    spinlock_release(atm->lock);
    return atm->value;
}

int atomic_load(atomic *atm){
    return atm->value;
}
