#ifndef _KERN_VMM_MPTFORK_H_
#define _KERN_VMM_MPTFORK_H_

#ifdef _KERN_

unsigned int get_curid(void);
void proc_start_user(void);
unsigned int thread_spawn(void *entry, unsigned int id, unsigned int quota);


unsigned int map_page(unsigned int proc_index, unsigned int vaddr, unsigned int page_index, unsigned int perm);
unsigned int get_pdir_entry(unsigned int proc_index, unsigned int pde_index);
void set_pdir_entry(unsigned int proc_index, unsigned int pde_index, unsigned int page_index);
unsigned int get_ptbl_entry(unsigned int proc_index, unsigned int pde_index, unsigned int pte_index);
void set_ptbl_entry(unsigned int proc_index, unsigned int pde_index, unsigned int pte_index, unsigned int page_index, unsigned int perm);

unsigned int get_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr);
void set_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr, unsigned int page_index, unsigned int perm);

unsigned int alloc_page(unsigned int proc_index, unsigned int vaddr, unsigned int perm);
void rmv_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr);

unsigned int container_alloc(unsigned int id);
unsigned int container_get_quota(unsigned int id);
unsigned int container_get_usage(unsigned int id);

unsigned int alloc_ptbl(unsigned int proc_index, unsigned int vaddr);
void rmv_pdir_entry(unsigned int proc_index, unsigned int pde_index);
void rmv_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index);

#endif  /* _KERN_ */

#endif  /* !_KERN_VMM_MPTINTRO_H_ */
