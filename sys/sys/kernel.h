#ifndef _KERNEL_H
#define _KERNEL_H
#include <sys/types.h>
#include <sys/proc.h>

typedef struct cpu_use {
	pid_t running;
	bool stop;
	pid_t start;
} cpu_use;


#endif
