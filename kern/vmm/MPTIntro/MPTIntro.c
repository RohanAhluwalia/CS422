#include <lib/gcc.h>
#include <lib/x86.h>
#include <lib/debug.h>

#include "import.h"

#define PT_PERM_UP  0
#define PT_PERM_PTU (PTE_P | PTE_W | PTE_U)


unsigned int getPageTableAddr(unsigned int page_index){
    return (page_index << 12); //shift right 12 bits
}

unsigned int removePERMBits(unsigned int addr){
    addr &= 0xfffff000;
    return addr;
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

    // Figure out start addr of page index
    unsigned int startAddr = getPageTableAddr(page_index);
    startAddr |= PT_PERM_PTU;  // add all of the permissions

    PDirPool[proc_index][pde_index]=(unsigned int*) startAddr;
}

// Sets the page directory entry # [pde_index] for the process # [proc_index]
// with the initial address of page directory # [pde_index] in IDPTbl.
// You should also set the permissions PTE_P, PTE_W, and PTE_U.
// This will be used to map a page directory entry to an identity page table.
void set_pdir_entry_identity(unsigned int proc_index, unsigned int pde_index)
{
    unsigned int addr = (unsigned int) IDPTbl[pde_index] | PT_PERM_PTU; 
    PDirPool[proc_index][pde_index]=(unsigned int*) addr;
}

// Removes the specified page directory entry (sets the page directory entry to 0).
// Don't forget to cast the value to (unsigned int *).
void rmv_pdir_entry(unsigned int proc_index, unsigned int pde_index)
{
    PDirPool[proc_index][pde_index]=(unsigned int*)0x00000000;
}

// Returns the specified page table entry.
// Do not forget that the permission info is also stored in the page directory entries.
unsigned int get_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                            unsigned int pte_index)
{

    unsigned int addr = (unsigned int) PDirPool[proc_index][pde_index]; //gets us the base address

    //need to remove the permision bits 
    addr = removePERMBits(addr);
    addr += pte_index << 2;

    return *(unsigned int *)addr;
}

// Sets the specified page table entry with the start address of physical page # [page_index]
// You should also set the given permission.
void set_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index, unsigned int page_index,
                    unsigned int perm)
{

    // finds the addresss of the start of the physical page
    unsigned int addr =  (unsigned int)PDirPool[proc_index][pde_index];

    //remove permission bits
    addr = removePERMBits(addr);
    addr += pte_index<<2;

    unsigned int pte_addr = getPageTableAddr(page_index);

    unsigned int *pte = (unsigned int *)addr;
    *pte = pte_addr | perm;
}

// Sets up the specified page table entry in IDPTbl as the identity map.
// You should also set the given permission.
void set_ptbl_entry_identity(unsigned int pde_index, unsigned int pte_index,
                             unsigned int perm)
{
    IDPTbl[pde_index][pte_index]=((pde_index<<10)+pte_index)<<12;
    IDPTbl[pde_index][pte_index]|=perm;
}

// Sets the specified page table entry to 0.
void rmv_ptbl_entry(unsigned int proc_index, unsigned int pde_index,
                    unsigned int pte_index)
{
    unsigned int addr = (unsigned int)PDirPool[proc_index][pde_index];
    addr = removePERMBits(addr);
    addr += pte_index << 2; 

    *((unsigned int *) addr) = 0; 
}
