#include <lib/trap.h>
#include <lib/debug.h>
#include <lib/string.h>
#include <dev/intr.h>
#include "import.h"

#define KERN_INFO_CPU(str, idx) \
    if (idx == 0) KERN_INFO("[BSP KERN] " str); \
    else KERN_INFO("[AP%d KERN] " str, idx);



//Define exceptions, interrupts, syscalls
#define EXC_START T_DIVIDE
#define EXC_END T_SECEV
#define INTR_START T_IRQ0 + IRQ_TIMER
#define INTR_END T_IRQ0 + IRQ_IDE2
#define SYSCALL T_SYSCALL

int inited = FALSE;

trap_cb_t TRAP_HANDLER[NUM_CPUS][256];

void trap_init_array(void)
{
    KERN_ASSERT(inited == FALSE);
    memzero(&TRAP_HANDLER, sizeof(trap_cb_t) * 8 * 256);
    inited = TRUE;
}

void trap_handler_register(int cpu_idx, int trapno, trap_cb_t cb)
{
    KERN_ASSERT(0 <= cpu_idx && cpu_idx < 8);
    KERN_ASSERT(0 <= trapno && trapno < 256);
    KERN_ASSERT(cb != NULL);

    TRAP_HANDLER[cpu_idx][trapno] = cb;
}

void trap_init(unsigned int cpu_idx)
{
    if (cpu_idx == 0) {
        trap_init_array();
    }

    KERN_INFO_CPU("Register trap handlers...\n", cpu_idx);

    // TODO: for CPU # [cpu_idx], register appropriate trap handler for each trap number,
    // with trap_handler_register function defined above.
    for (unsigned int trapno = 0; trapno < 256; trapno++)
    {
        // exception
        if (EXC_START <= trapno && trapno <= EXC_END)
        {
            trap_handler_register(cpu_idx, trapno, &exception_handler);
        }
        // interrupt
        else if (INTR_START <= trapno && trapno <= INTR_END)
        {
            trap_handler_register(cpu_idx, trapno, &interrupt_handler);
        }
        // syscall dispatch
        else if (trapno == T_SYSCALL)
        {
            trap_handler_register(cpu_idx, trapno, &syscall_dispatch);
        }
    }


    KERN_INFO_CPU("Done.\n", cpu_idx);
    KERN_INFO_CPU("Enabling interrupts...\n", cpu_idx);

    /* enable interrupts */
    intr_enable(IRQ_TIMER, cpu_idx);
    intr_enable(IRQ_KBD, cpu_idx);
    intr_enable(IRQ_SERIAL13, cpu_idx);

    KERN_INFO_CPU("Done.\n", cpu_idx);
}
