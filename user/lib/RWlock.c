#include <RWlock.h> 

#define FUTEX_WAIT 1
#define FUTEX_WAKE 2
#define FUTEX_RENQ 3

#define cmpxchg(P, O, N) __sync_val_compare_and_swap((P), (O), (N))

static unsigned _lock = 1; // read-write lock futex
const static unsigned _lock_open = 1;
const static unsigned _lock_wlocked = 0;

static void _unlock(){
    unsigned current, wanted;
    do {
        current = _lock;
        if (current == _lock_open) return;
        if (current == _lock_wlocked) {
            wanted = _lock_open;
        } else {
            wanted = current - 1;
        }
    } while (cmpxchg(&_lock, current, wanted) != current);
    sys_futex((int*)&_lock, FUTEX_WAKE, 1, 0, NULL);
}

static void _rlock(){
    unsigned current;
    while ((current = _lock) == _lock_wlocked || cmpxchg(&_lock, current, current + 1) != current) {
        while (sys_futex((int*)&_lock, FUTEX_WAIT, current, 0, NULL) != 0) {
            if (_lock >= _lock_open) break;
        }
        // will always be able to acquire Rlock
    }
}

static void _wlock()
{
    unsigned current;
    while ((current = cmpxchg(&_lock, _lock_open, _lock_wlocked)) != _lock_open) {
        while (sys_futex((int*)&_lock, FUTEX_WAIT, current, 0, NULL) != 0) {
            if (_lock == _lock_open) break;
        }
        if (_lock != _lock_open) {
            // wake someone else because it's in RLOCK
            sys_futex((int*)&_lock, FUTEX_WAKE, 1, 0, NULL);
        }
    }
}


