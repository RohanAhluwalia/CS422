#include <lib/x86.h>
#include <lib/string.h>
#include <lib/trap.h>
#include <lib/syscall.h>

#include "import.h"

#define PERM_PTU (PTE_P | PTE_W | PTE_U)
#define PERM_PTC (PTE_P | PTE_W | PTE_COW)

#define PAGESIZE 4096
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)
#define PDIRSIZE      (PAGESIZE * 1024)
#define VM_USERLO_PDE (VM_USERLO / PDIRSIZE)
#define VM_USERHI_PDE (VM_USERHI / PDIRSIZE)

void copy_ptbl(unsigned int src, unsigned int dst){

    unsigned int pte_index, pde_index, ptbl_entry, phys_page_index, vaddr, perm;

    //1. iterate through the pde index
    for (pde_index = 0; pde_index < 1024; pde_index++){

        // Allocate ptbl if is a valid pde_index
        if (pde_index >= VM_USERLO_PDE && pde_index < VM_USERHI_PDE){

            unsigned int pde = get_pdir_entry(src, pde_index);

            //check valid
            if(pde == 0){
                rmv_pdir_entry(src, pde_index);
            }else{
                unsigned int ptbl_index = pde_index << 22;
                alloc_ptbl(dst, ptbl_index);
                
                //3. Iterate through pte_index
                for (pte_index = 0; pte_index < 1024; pte_index++){
                    unsigned int ptbl_entry = get_ptbl_entry(src, pde_index, pte_index);
                    
                    // check valid
                    if(ptbl_entry == 0){
                        rmv_ptbl_entry(dst, pde_index, pte_index);
                    }else{
                        // check perms
                        phys_page_index = ptbl_entry >> 12;
                        perm = ptbl_entry & 0xFFF; // from office hours
                        vaddr = ((pde_index << 10) + pte_index) << 12; // calculate for map_page

                        // if writable or copy on write 
                        if ((perm & PERM_PTU) == PERM_PTU || (perm & PERM_PTC) == (PERM_PTC)){
                            perm = (perm & ~PTE_W) | PTE_COW; // make permisions copy on write
                            map_page(src, vaddr, phys_page_index, perm);
                        }
                        map_page(dst, vaddr, phys_page_index, perm);
                    }

                }
            }
        }

    }

}


extern tf_t uctx_pool[NUM_IDS];
unsigned int proc_fork(void)
{
    unsigned int child, parent, quota;

    parent = get_curid();
    
    // return invalid if not enough space
    quota = (container_get_quota(parent) - container_get_usage(parent)) / 2;
    if (quota == 0)
    {
        return NUM_IDS;
    }

    // create child process
    child = thread_spawn((void *)proc_start_user, parent, quota);
    if (child == NUM_IDS)
    {
        return child;
    }

    // copy on write (re-map the page table to the right perms)
    copy_ptbl(parent, child);

    // copy
    memcpy((void *)(&uctx_pool[child]), (void *)(&uctx_pool[parent]), sizeof(tf_t));

    // set to success (should do it already but to double check)
    uctx_pool[child].regs.eax = E_SUCC; 
    uctx_pool[child].regs.ebx = 0;     

    return child;
}



void copy_on_write(unsigned int id, unsigned int vaddr)
{
    unsigned int page_index, ptbl_entry;
    void * src_paddr = (void *) (get_ptbl_entry_by_va(id, vaddr) & ~0xFFF);

    ptbl_entry = get_ptbl_entry_by_va(id, vaddr);
    rmv_ptbl_entry_by_va(id, vaddr);

    page_index = container_alloc(id);
    if (page_index == 0)
    {
        return;
    }

    //copy content
    void * dst_paddr = (void *) (page_index << 12);
    memcpy(dst_paddr, src_paddr, PAGESIZE);
    
    //update perms
    map_page(id, vaddr, page_index, PERM_PTU);
}