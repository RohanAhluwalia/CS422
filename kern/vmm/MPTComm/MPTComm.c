#include <lib/x86.h>

#include "import.h"


void memorymap_process(int pid) {
    for(int i = 0; i < 1024; i++) {
        for(int j = 0; j < 1024; j++) {
            unsigned int pa = construct_pa_from_indices(i, j);
            unsigned int perm = set_pwu_bits(0, 1, 1, 1);
            if(pa < VM_USERLO_PI || pa >= VM_USERHI_PI) {
                set_ptbl_entry(pid, i, j, pa, perm)
                perm = (unsigned int)set_pwu_bits(perm, 1, 1, 1);
            }
        }
    }
}

void clear_ptbl(unsigned int proc_index, unsigned int pde_index) {
    for(int i = 0; i < 1024; i++) {
        rmv_ptbl_entry(proc_index, pde_index);
    }
}
/**
 * For each process from id 0 to NUM_IDS - 1,
 * set up the page directory entries so that the kernel portion of the map is
 * the identity map, and the rest of the page directories are unmapped.
 */
void pdir_init(unsigned int mbi_addr)
{
    // TODO: Define your local variables here.

    idptbl_init(mbi_addr);
    for(int i = 0; i < NUM_IDS) {
        memorymap_process(i);
    }
    // TODO
}

/**
 * Allocates a page (with container_alloc) for the page table,
 * and registers it in the page directory for the given virtual address,
 * and clears (set to 0) all page table entries for this newly mapped page table.
 * It returns the page index of the newly allocated physical page.
 * In the case when there's no physical page available, it returns 0.
 */
unsigned int alloc_ptbl(unsigned int proc_index, unsigned int vaddr)
{
    // TODO
    PAGE_ENTRY directory_index = get_directory_index(vaddr);
    PAGE_ENTRY page_index = get_page_index(vaddr);
    unsigned int new_page = container_alloc(proc_index);
    if(new_page == 0) 
    {
        return 0;
    }


    return 0;
}

// Reverse operation of alloc_ptbl.
// Removes corresponding the page directory entry,
// and frees the page for the page table entries (with container_free).
void free_ptbl(unsigned int proc_index, unsigned int vaddr)
{
    // TODO
}
