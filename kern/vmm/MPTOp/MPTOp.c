#include <lib/x86.h>

#include "import.h"

#define VM_USERLO    0x40000000
#define VM_USERHI    0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)

typedef unsigned int* PAGE_ENTRY;

bool read_bit(unsigned int number, int n) {
    return number & ((unsigned int)1 << n);
}

unsigned int get_directory_index(unsigned int vaddr) {
    //dprintf("Vaddr, Dir Index | %d : %d\n", vaddr, ((unsigned int)void_n_bits(vaddr, 22, 0)));
    return void_n_bits(vaddr, 22, 0);
}

unsigned int get_page_index(unsigned int vaddr) {
    unsigned int dir = void_n_bits(vaddr, 22, 0);
    // We remove the offset from both. Further, we xor the page + dir with dir so that we're left with the
    // middle page bits. We must shift those 12 to the left to make up for the loss of dir and ensure the page 
    // is placed in front.
    //dprintf("Vaddr, Page Addr | %d : %d\n", vaddr, ((unsigned int)void_n_bits(vaddr, 12, 0) ^ dir));
    return ((unsigned int)void_n_bits(vaddr, 12, 0) ^ dir);
}

/**
 * Returns the page table entry corresponding to the virtual address,
 * according to the page structure of process # [proc_index].
 * Returns 0 if the mapping does not exist.
 */
unsigned int get_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    
    
    //PAGE_ENTRY offset = void_n_bits(vaddr, 20, 12);

    // Use the page section of the vaddr to obtain 
    PAGE_ENTRY directory_index = get_directory_index(vaddr);
    PAGE_ENTRY page_index = get_page_index(vaddr);
    if(read_bit(get_pdir_entry(proc_index, directory_index), 0) == 0) { 
        return 0;
    }

    PAGE_ENTRY ptable_entry = get_ptbl_entry(proc_index, directory_index, page_index);

    if(read_bit(ptable_entry, 0) == 0) {
        return 0;
    }

    return ptable_entry;

}

// Returns the page directory entry corresponding to the given virtual address.
unsigned int get_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    // TODO
    PAGE_ENTRY directory_index = get_directory_index(vaddr);
    return get_pdir_entry(proc_index, directory_index);
}

// Removes the page table entry for the given virtual address.
void rmv_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    // TODO
    PAGE_ENTRY directory_index = get_directory_index(vaddr);
    PAGE_ENTRY page_index = get_page_index(vaddr);

    rmv_ptbl_entry(proc_index, directory_index, page_index);
}

// Removes the page directory entry for the given virtual address.
void rmv_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr)
{
    // TODO
    PAGE_ENTRY directory_index = get_directory_index(vaddr);
    rmv_pdir_entry(proc_index, directory_index);
    
}

// Maps the virtual address [vaddr] to the physical page # [page_index] with permission [perm].
// You do not need to worry about the page directory entry. just map the page table entry.
void set_ptbl_entry_by_va(unsigned int proc_index, unsigned int vaddr,
                          unsigned int page_index, unsigned int perm)
{
    PAGE_ENTRY directory_index = get_directory_index(vaddr);
    PAGE_ENTRY pte_index = get_page_index(vaddr);

    set_ptbl_entry(proc_index, directory_index,
                    pte_index, page_index, perm);
}


// Registers the mapping from [vaddr] to physical page # [page_index] in the page directory.
void set_pdir_entry_by_va(unsigned int proc_index, unsigned int vaddr,
                          unsigned int page_index)
{
    // TODO
    PAGE_ENTRY directory_index = get_directory_index(vaddr);
    set_pdir_entry(proc_index, directory_index, page_index);
}

// Initializes the identity page table.
// The permission for the kernel memory should be PTE_P, PTE_W, and PTE_G,
// While the permission for the rest should be PTE_P and PTE_W.
void idptbl_init(unsigned int mbi_addr)
{
    // TODO: Define your local variables here.

    // TODO: Define your local variables here.

    container_init(mbi_addr);

    // TODO
    unsigned int vaddr = 0;
    unsigned int pde_index, pte_index, perm;
    set_pdir_base(0);
    for(int pde_index = 0; pde_index < 1024; pde_index++){
      for(int pte_index = 0; pte_index < 1024; pte_index++){
          vaddr = ((pde_index<<10)+pte_index)<<12;
          if(vaddr>=VM_USERLO && vaddr <VM_USERHI)perm = PTE_P|PTE_W;
          else perm = PTE_P|PTE_W|PTE_G;
          set_ptbl_entry_identity(pde_index, pte_index, perm);
      }
    }
    
    // container_init(mbi_addr);

    // for(int i = 0; i < 1024; i++) {
    //     for(int j = 0; j < 1024; j++) {
    //         unsigned int pa = construct_pa_from_indices(i, j);
    //         unsigned int perm = 0;
    //         if(pa < VM_USERLO_PI || pa >= VM_USERHI_PI) {
    //             perm = (unsigned int)set_pwu_bits(perm, 1, 1, 1);
    //         }
    //         else {
    //             perm = (unsigned int)set_pwu_bits(perm, 1, 1, 0);
    //         }
    //         set_ptbl_entry_identity(i, j, perm);
    //     }
    // }
}
