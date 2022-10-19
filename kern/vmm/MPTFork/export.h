#ifndef _KERN_VMM_MPTFORK_H_
#define _KERN_VMM_MPTFORK_H_

#ifdef _KERN_

void copy_ptbl(unsigned int proc_1, unsigned int proc_2);
unsigned int proc_fork(void);
void copy_on_write(unsigned int pid, unsigned int vaddr);

#endif  /* _KERN_ */

#endif  /* !_KERN_VMM_MPTFORK_H_ */