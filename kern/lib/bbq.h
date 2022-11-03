#ifndef _KERN_LIB_BBQ_H_
#define _KERN_LIB_BBQ_H_

#ifdef _KERN_

#include <lib/gcc.h>
#include <lib/types.h>
#include <lib/x86.h>
#include <lib/mqueue.h>
#include <lib/cv.h>

#define BBQ_SIZE 8

/**
 * Queue implementation using an array
 * Assumes there can be at most NUM_IDS
 * cpus waiting simultaneously
*/
#define BUFF_SIZE NUM_IDS

typedef struct
{
    unsigned int queue[BBQ_SIZE];
    unsigned int head;
    unsigned int next;
    spinlock_t lock;
    cv_t added;
    cv_t removed;
} bbq_t;

extern bbq_t shared_bbq;

void bbq_init(bbq_t *bbq);
void bbq_insert(bbq_t *bbq, unsigned int insert);
unsigned int bbq_remove(bbq_t *bbq);

#endif /* _KERN_ */

#endif /* !_KERN_LIB_CVAR_H_ */