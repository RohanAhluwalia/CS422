#include <lib/x86.h>

#include "import.h"

#define VM_USERLO    0x40000000
#define VM_USERHI    0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)

/**
 * Helper function that determines the page index from virtual memory
 */
unsigned int get_page_index(unsigned int vaddr) {
    return (vaddr / PAGESIZE) % 1024; //divide by the pagesize then find the correct page
}

/**
 * Returns the page table entry corresponding to the virtual address,
 * according to the page structure of process # [proc_index].
 * Returns 0 if the mapping does not exist.
 */
unsigned int get_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    
    unsigned int pde_index;
    unsigned int page_index;
    unsigned int ptbl_entry;


    pde_index = vaddr >> 22; // (4096 * 1024)

    unsigned int pde = get_pdir_entry(proc_index, pde_index);
    if (pde == 0) return 0;

    page_index = get_page_index(vaddr); // find the start of the page table entry

    ptbl_entry = get_ptbl_entry(proc_index, pde_index, page_index);
    return ptbl_entry;
}

// Returns the page directory entry corresponding to the given virtual address.
unsigned int get_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int pde_index = vaddr >> 22;
    return get_pdir_entry(proc_index,pde_index);
}

// Removes the page table entry for the given virtual address.
void rmv_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    unsigned int pde_index = vaddr >> 22;
    unsigned int page_index = get_page_index(vaddr); 
    rmv_ptbl_entry(proc_index, pde_index, page_index);
}

// Removes the page directory entry for the given virtual address.
void rmv_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    // TODO
    unsigned int pde_index = vaddr >> 22;
    rmv_pdir_entry(proc_index, pde_index);
    
}

// Maps the virtual address [vaddr] to the physical page # [page_index] with permission [perm].
// You do not need to worry about the page directory entry. just map the page table entry.
void set_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr,
                          unsigned int page_index, unsigned int perm)
{
    unsigned int pde_index = vaddr >> 22;
    unsigned int pte_index = get_page_index(vaddr);

    set_ptbl_entry(proc_index, pde_index, pte_index, page_index, perm);
}


// Registers the mapping from [vaddr] to physical page # [page_index] in the page directory.
void set_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr,
                          unsigned int page_index)
{
    unsigned int pde_index = vaddr >> 22;
    set_pdir_entry(proc_index, pde_index, page_index);
}

// Initializes the identity page table.
// The permission for the kernel memory should be PTE_P, PTE_W, and PTE_G,
// While the permission for the rest should be PTE_P and PTE_W.
void idptbl_init(unsigned int mbi_addr)
{

    container_init(mbi_addr);
    set_pdir_base(0);
    for(int i = 0; i < 1024; i++) {
        for(int j = 0; j < 1024; j++) {
            unsigned int vaddr = ((i<<10)+j)<<12;
            unsigned int perm = 0;
            if(vaddr < VM_USERHI && vaddr >= VM_USERLO) {
                perm = PTE_P | PTE_W;
            }
            else {
                perm = PTE_P | PTE_W | PTE_G;
            }
            set_ptbl_entry_identity(i, j, perm);
        }
    }
}
