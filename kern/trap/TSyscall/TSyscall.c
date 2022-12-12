#include <lib/debug.h>
#include <lib/pmap.h>
#include <lib/types.h>
#include <lib/x86.h>
#include <lib/trap.h>
#include <lib/string.h>
#include <lib/syscall.h>
#include <dev/console.h>
#include <dev/intr.h>
#include <pcpu/PCPUIntro/export.h>
#include <kern/thread/PThread/export.h>
#include <kern/lib/spinlock.h>

void thread_sleep(void *chan, spinlock_t *lk);
void thread_wakeup(void *chan);
void spinlock_init(spinlock_t *lk);
#include "import.h"

#define BUFLEN 1024  // from kern/dev/console.c
static char sys_buf[NUM_IDS][PAGESIZE];
static spinlock_t futex_spinlock;

/**
 * Copies a string from user into buffer and prints it to the screen.
 * This is called by the user level "printf" library as a system call.
 */
void sys_puts(tf_t *tf)
{
    unsigned int cur_pid;
    unsigned int str_uva, str_len;
    unsigned int remain, cur_pos, nbytes;

    cur_pid = get_curid();
    str_uva = syscall_get_arg2(tf);
    str_len = syscall_get_arg3(tf);

    if (!(VM_USERLO <= str_uva && str_uva + str_len <= VM_USERHI)) {
        syscall_set_errno(tf, E_INVAL_ADDR);
        return;
    }

    remain = str_len;
    cur_pos = str_uva;

    while (remain) {
        if (remain < PAGESIZE - 1)
            nbytes = remain;
        else
            nbytes = PAGESIZE - 1;

        if (pt_copyin(cur_pid, cur_pos, sys_buf[cur_pid], nbytes) != nbytes) {
            syscall_set_errno(tf, E_MEM);
            return;
        }

        sys_buf[cur_pid][nbytes] = '\0';
        KERN_INFO("%s", sys_buf[cur_pid]);

        remain -= nbytes;
        cur_pos += nbytes;
    }

    syscall_set_errno(tf, E_SUCC);
}

void sys_readline(tf_t *tf)
{
    char *buf;
    int read;
    unsigned int curid = get_curid();
    uintptr_t line = syscall_get_arg2(tf);
    uintptr_t len = syscall_get_arg3(tf);

    if (!(VM_USERLO <= line && line + len <= VM_USERHI)
        || len >= BUFLEN) {
        syscall_set_errno(tf, E_INVAL_ADDR);
        return;
    }

    buf = readline("$> ");
    len = min(strnlen(buf, BUFLEN - 1), len);
    buf[len] = '\0';
    read = pt_copyout(buf, curid, line, len + 1);
    if (len > 0 && read == 0) {
        syscall_set_errno(tf, E_MEM);
        return;
    }

    syscall_set_errno(tf, E_SUCC);
    syscall_set_retval1(tf, read);
}

extern uint8_t _binary___obj_user_pingpong_ping_start[];
extern uint8_t _binary___obj_user_pingpong_pong_start[];
extern uint8_t _binary___obj_user_pingpong_ding_start[];
extern uint8_t _binary___obj_user_fstest_fstest_start[];
extern uint8_t _binary___obj_user_shell_shell_start[];

/**
 * Spawns a new child process.
 * The user level library function sys_spawn (defined in user/include/syscall.h)
 * takes two arguments [elf_id] and [quota], and returns the new child process id
 * or NUM_IDS (as failure), with appropriate error number.
 * Currently, we have three user processes defined in user/pingpong/ directory,
 * ping, pong, and ding.
 * The linker ELF addresses for those compiled binaries are defined above.
 * Since we do not yet have a file system implemented in mCertiKOS,
 * we statically load the ELF binaries into the memory based on the
 * first parameter [elf_id].
 * For example, ping, pong, and ding correspond to the elf_ids
 * 1, 2, 3, and 4, respectively.
 * If the parameter [elf_id] is none of these, then it should return
 * NUM_IDS with the error number E_INVAL_PID. The same error case apply
 * when the proc_create fails.
 * Otherwise, you should mark it as successful, and return the new child process id.
 */
void sys_spawn(tf_t *tf)
{
    unsigned int new_pid;
    unsigned int elf_id, quota;
    void *elf_addr;
    unsigned int curid = get_curid();

    elf_id = syscall_get_arg2(tf);
    quota = syscall_get_arg3(tf);

    if (!container_can_consume(curid, quota)) {
        syscall_set_errno(tf, E_EXCEEDS_QUOTA);
        syscall_set_retval1(tf, NUM_IDS);
        return;
    }
    else if (NUM_IDS < curid * MAX_CHILDREN + 1 + MAX_CHILDREN) {
        syscall_set_errno(tf, E_MAX_NUM_CHILDEN_REACHED);
        syscall_set_retval1(tf, NUM_IDS);
        return;
    }
    else if (container_get_nchildren(curid) == MAX_CHILDREN) {
        syscall_set_errno(tf, E_INVAL_CHILD_ID);
        syscall_set_retval1(tf, NUM_IDS);
        return;
    }

    switch (elf_id) {
    case 1:
        elf_addr = _binary___obj_user_pingpong_ping_start;
        break;
    case 2:
        elf_addr = _binary___obj_user_pingpong_pong_start;
        break;
    case 3:
        elf_addr = _binary___obj_user_pingpong_ding_start;
        break;
    case 4:
        elf_addr = _binary___obj_user_fstest_fstest_start;
        break;
    case 5:
        elf_addr = _binary___obj_user_shell_shell_start;
        break;
    default:
        syscall_set_errno(tf, E_INVAL_PID);
        syscall_set_retval1(tf, NUM_IDS);
        return;
    }

    new_pid = proc_create(elf_addr, quota);

    if (new_pid == NUM_IDS) {
        syscall_set_errno(tf, E_INVAL_PID);
        syscall_set_retval1(tf, NUM_IDS);
    } else {
        syscall_set_errno(tf, E_SUCC);
        syscall_set_retval1(tf, new_pid);
    }
}

/**
 * Yields to another thread/process.
 * The user level library function sys_yield (defined in user/include/syscall.h)
 * does not take any argument and does not have any return values.
 * Do not forget to set the error number as E_SUCC.
 */
void sys_yield(tf_t *tf)
{
    thread_yield();
    syscall_set_errno(tf, E_SUCC);
}


void futex_init() {
    spinlock_init(&futex_spinlock);
    KERN_DEBUG("SUCCESSFULLY INITIALIZED FUTEX SPINLOCK\n");

}

void sys_futex(tf_t * tf) {
    // Get the passed parameters.
    KERN_DEBUG("Futex Called!\n");

    int * uaddr = syscall_get_arg2(tf);
    int futex_op = syscall_get_arg3(tf);
    int val = syscall_get_arg4(tf);
    int val2 = syscall_get_arg5(tf);
    int* new_uaddr = syscall_get_arg6(tf);

    spinlock_acquire(&futex_spinlock);

    /* Wait Operation */
    if(futex_op == 1) {
        // If the address uses the expected value, we wait on it.
        if(*((unsigned int*)uaddr) == val) {
            thread_sleep(uaddr, &futex_spinlock);
        }
    }
    else if(futex_op == 2) {
        thread_wakeup_limited(uaddr, val);
    }
    else if(futex_op == 4) {
        if(*((unsigned int*)uaddr) == val) {
            thread_sleep(uaddr, &futex_spinlock);
            thread_requeue_limited(uaddr, val2, new_uaddr);
        }
        else {
            syscall_set_errno(tf, E_AGAIN);
            syscall_set_retval1(tf, NUM_IDS);
        }
    }
    spinlock_release(&futex_spinlock);


}