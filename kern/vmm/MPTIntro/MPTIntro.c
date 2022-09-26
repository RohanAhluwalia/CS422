#include <lib/gcc.h>
#include <lib/x86.h>
#include <lib/debug.h>

#include "import.h"

#define PT_PERM_UP  0
#define PT_PERM_PTU (PTE_P | PTE_W | PTE_U)
typedef unsigned int* PAGE_ENTRY;


/** 
 * Helper function to set the nth bit of a given page entry. 
 * n - the 0-indexed bit to set.
 * value - the binary value (0 or 1) to assign to the given place.
*/
PAGE_ENTRY set_nth_bit (PAGE_ENTRY number, unsigned int n, bool value) {

    if(value == 1) {
        return (unsigned int)number | ((unsigned int)(1) << n);
    }
    if(value == 0) {
        return (unsigned int)number & ~((unsigned int)(1) << n);
    }
}

PAGE_ENTRY set_pwu_bits(PAGE_ENTRY entry, bool present_value, bool write_value, bool user_value) {
    dprintf('Setting PWU bits For: %d\n', entry);
    entry = set_nth_bit(entry, 0, present_value);
    entry = set_nth_bit(entry, 1, write_value);
    entry = set_nth_bit(entry, 2, user_value);
    return entry;
}


PAGE_ENTRY void_n_bits(PAGE_ENTRY entry, int n, int start) {
    unsigned int og = entry;
    //dprintf("Voiding Entry %d\n", entry);

    for(int i = start; i < n; i++) {
        entry = set_nth_bit(entry, i, 0);
    }
    //dprintf("Voided Entry  %d\n", entry);
    return entry;
}


PAGE_ENTRY get_pt_entry_mem_location(PAGE_ENTRY pdir_entry, PAGE_ENTRY pte_index) {
    // Assumption is that the table addresses are laid out linearly from the address entry. If
    // we dereference this pointer, we should be at the correct location?
    dprintf("Memory Location: %d : %d\n", pdir_entry, pte_index);
    return (unsigned int)void_n_bits(pdir_entry, 12, 0) + (unsigned int)(pte_index);
}

unsigned int construct_pa_from_indices(unsigned int pde_index, unsigned int pte_index) {
    return (pde_index*PAGESIZE) << 22 | (pte_index*PAGESIZE) << 12;
}
/**
 * Page directory pool for NUM_IDS processes.
 * mCertiKOS maintains one page structure for each process.
 * Each PDirPool[index] represents the page directory of the page structure
 * for the process # [index].
 * In mCertiKOS, we statically allocate page directories, and maintain the second
 * level page tables dynamically.
 * The unsigned int * type is meant to suggest that the contents of the array
 * are pointers to page tables. In reality they are actually page directory
 * entries, which are essentially pointers plus permission bits. The functions
 * in this layer will require casting between integers and pointers anyway and
 * in fact any 32-bit type is fine, so feel free to change it if it makes more
 * sense to you with a different type.
 */
unsigned int *PDirPool[NUM_IDS][1024] gcc_aligned(PAGESIZE);

/**
 * In mCertiKOS, we use identity page table mappings for the kernel memory.
 * IDPTbl is an array of statically allocated identity page tables that will be
 * reused for all the kernel memory.
 * That is, in every page directory, the entries that fall into the range of
 * addresses reserved for the kernel will point to an entry in IDPTbl.
 */
unsigned int IDPTbl[1024][1024] gcc_aligned(PAGESIZE);

// Sets the CR3 register with the start address of the page structure for process # [index].
void set_pdir_base(unsigned int index)
{
    // TODO
    set_cr3(PDirPool[index]);
}

// Returns the page directory entry # [pde_index] of the process # [proc_index].
// This can be used to test whether the page directory entry is mapped.
unsigned int get_pdir_entry(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    return (unsigned int)PDirPool[proc_index][pde_index];
}

// Sets the specified page directory entry with the start address of physical
// page # [page_index].
// You should also set the permissions PTE_P, PTE_W, and PTE_U.
void set_pdir_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int page_index)
{
    // TODO
    PDirPool[proc_index][pde_index] = set_pwu_bits(page_index*PAGESIZE, 1, 1, 1);
}

// Sets the page directory entry # [pde_index] for the process # [proc_index]
// with the initial address of page directory # [pde_index] in IDPTbl.
// You should also set the permissions PTE_P, PTE_W, and PTE_U.
// This will be used to map a page directory entry to an identity page table.
void set_pdir_entry_identity(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    PDirPool[proc_index][pde_index] = set_pwu_bits(IDPTbl[pde_index], 1, 1, 1);
    //dprintf("%d : %d : %d\n", pde_index, PDirPool[proc_index][pde_index], IDPTbl[pde_index]);
    //dprintf("Sizeofs: %d : %d\n", sizeof(PAGE_ENTRY), sizeof(unsigned int));

}

// Removes the specified page directory entry (sets the page directory entry to 0).
// Don't forget to cast the value to (unsigned int *).
void rmv_pdir_entry(unsigned int proc_index, unsigned int pde_index)
{
    // TODO
    PDirPool[proc_index][pde_index] = (PAGE_ENTRY)0;
}

// Returns the specified page table entry.
// Do not forget that the permission info is also stored in the page directory entries.
unsigned int get_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                            unsigned int pte_index)
{
    // TODO

    // This gives us the address in heap memory of the desired page table. 
    PAGE_ENTRY pdir_entry = (PAGE_ENTRY)get_pdir_entry(proc_index, pde_index);

    // Assumption is that the table addresses are laid out linearly from the address entry. If
    // we dereference this pointer, we should be at the correct location?
    return *(get_pt_entry_mem_location(pdir_entry, pte_index));
}

// Sets the specified page table entry with the start address of physical page # [page_index]
// You should also set the given permission.
void set_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index, unsigned int page_index,
                    unsigned int perm)
{
    // TODO: Check 
    PAGE_ENTRY pdir_entry = (PAGE_ENTRY)get_pdir_entry(proc_index, pde_index);
    *(get_pt_entry_mem_location(pdir_entry, pte_index)) = (PAGE_ENTRY)((page_index * PAGESIZE) | perm);
}

// Sets up the specified page table entry in IDPTbl as the identity map.
// You should also set the given permission.
void set_ptbl_entry_identity(unsigned int pde_index, unsigned int pte_index,
                             unsigned int perm)
{
    // TODO
    IDPTbl[pde_index][pte_index] = (PAGE_ENTRY)(construct_pa_from_indices(pde_index, pte_index) | perm);
}

// Sets the specified page table entry to 0.
void rmv_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index)
{
    // TODO
    PAGE_ENTRY pdir_entry = get_pdir_entry(proc_index, pde_index);
    *(get_pt_entry_mem_location(pdir_entry, pte_index)) = (PAGE_ENTRY)0;
}
