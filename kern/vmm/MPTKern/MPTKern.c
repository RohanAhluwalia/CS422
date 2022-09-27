#include <lib/x86.h>
#include <lib/debug.h>

#include "import.h"

/**
 * Sets the entire page map for process 0 as the identity map.
 * Note that part of the task is already completed by pdir_init.
 */
void pdir_init_kern(unsigned int mbi_addr)
{
    // TODO: Define your local variables here.

    pdir_init(mbi_addr);

    unsigned int pde_index = 0;
    for (; pde_index < 1024; pde_index++){
        set_pdir_entry_identity(0, pde_index);
    }
    //TODO
}

/**
 * Maps the physical page # [page_index] for the given virtual address with the given permission.
 * In the case when the page table for the page directory entry is not set up,
 * you need to allocate the page table first.
 * In the case of error, it returns the constant MagicNumber defined in lib/x86.h,
 * otherwise, it returns the physical page index registered in the page directory,
 * (the return value of get_pdir_entry_by_va or alloc_ptbl).
 */
unsigned int map_page(unsigned int proc_index, unsigned int vaddr,
                      unsigned int page_index, unsigned int perm)
{
    unsigned int pde = get_pdir_entry_by_va(proc_index, vaddr);
    unsigned int ptbl; 

    if ((pde & PTE_P) == 0){
        ptbl = alloc_ptbl(proc_index, vaddr);
        if (ptbl == 0){
            return MagicNumber; // no physical page avaliable (ASK)
        }
    }

    set_ptbl_entry_by_va(proc_index, vaddr, page_index, perm);
    pde = get_pdir_entry_by_va(proc_index, vaddr);

    return pde >> 12;
}

/**
 * Remove the mapping for the given virtual address (with rmv_ptbl_entry_by_va).
 * You need to first make sure that the mapping is still valid,
 * e.g., by reading the page table entry for the virtual address.
 * Nothing should be done if the mapping no longer exists.
 * You do not need to unmap the page table from the page directory.
 * It should return the corresponding page table entry.
 */
unsigned int unmap_page(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int pte = get_ptbl_entry_by_va(proc_index, vaddr);
    if ((pte & PTE_P) == 0){
        return pte;
    }

    rmv_ptbl_entry_by_va(proc_index, vaddr);
    pte = get_ptbl_entry_by_va(proc_index, vaddr);
    
    return pte;
}
