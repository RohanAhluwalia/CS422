// Physical memory management definitions.
// See COPYRIGHT for copyright information.
#ifndef PIOS_KERN_MEM_H
#define PIOS_KERN_MEM_H

#include <architecture/types.h>
#include <inc/pmem_layout.h>


// At physical address MEM_IO (640K) there is a 384K hole for I/O.
// The hole ends at physical address MEM_EXT, where extended memory begins.
#define MEM_IO		0x0A0000
#define MEM_EXT		0x100000



//some consts for svm
#define START_PMEM_VMM 0x8000000
#define HOST_PMEM_SIZE 0xF000000
#define GUEST_PMEM_SIZE 0x4000000
#define GUEST_FIXED_PMEM_BYTES 0x4000000
#define PIOS_PMEM_START 0x100000
#define PAGE_SHIFT 12
#define PAGE_SIZE 4096

#define STACK_SIZE	(1 << 16) /* 64 KB */
#define	DEFAULT_VMM_PMEM_SIZE (1 << 24) /* 16 MB */
#define	DEFAULT_VMM_PMEM_START  0x100000// (1 << 20) /* 1 MB */

#define PAGE_SHIFT_4MB 22
#define PAGE_SIZE_4MB  ( 1 << PAGE_SHIFT_4MB )
#define PAGE_MASK_4MB  ( ~ ( PAGE_SIZE_4MB - 1 ) )

#define PAGE_SHIFT_2MB 21
#define PAGE_SIZE_2MB  ( 1 << PAGE_SHIFT_2MB )
//haind for 4kb page
#define PAGE_SHIFT_4KB 12
#define PAGE_SIZE_4KB (1 << PAGE_SHIFT_4KB)


#define PFN_UP_2MB(x)   (((x) + PAGE_SIZE_2MB - 1) >> PAGE_SHIFT_2MB)
#define PFN_DOWN_2MB(x) ((x) >> PAGE_SHIFT_2MB)


#define PAGE_MASK  ( ~ ( PAGE_SIZE - 1 ) )
#define PFN_UP(x)       (((x) + PAGE_SIZE - 1) >> PAGE_SHIFT)
#define PFN_DOWN(x)     ((x) >> PAGE_SHIFT)
#define PFN_PHYS(x)     ((x) << PAGE_SHIFT)

#define PAGE_UP(p)    ( ( (p) + ( PAGE_SIZE - 1 ) ) & PAGE_MASK )       // round up to closest next 4KB page boundary
#define PAGE_DOWN(p)  ( (p) & PAGE_MASK )               // round down to closest previous 4KB page boundary

#define PAGE_UP_4MB(p)    ( ( (p) + ( PAGE_SIZE_4MB - 1 ) ) & PAGE_MASK_4MB )   // round up to closest next 4MB page boundary
#define PAGE_DOWN_4MB(p)  ( (p) & PAGE_MASK_4MB )               // round down to closest previous 4MB page boundary

enum pg_table_level {
        PGT_LEVEL_PML4 = 4,
        PGT_LEVEL_PDP  = 3,
        PGT_LEVEL_PD   = 2,
        PGT_LEVEL_PT   = 1 //haind for page table
};

/*Anh - For 4MB page translation, PAE disabled, vol2 p124  */
struct pd4M_entry {
        uint16_t flags:      13; /* Bit 0-12 */
        uint32_t basehigh:   8;      /* Bit 13-20 of the entry => bit 32-39 of base */
        uint8_t  rsvr:               1;      /* Bit 21 */
        uint16_t baselow:    10;     /* Bit 22-31 of the entry => bit 22-31 of base */
} __attribute__ ((packed)) pd4M_entry;

/*haind - For 4KB page translation, PAE disabled*/
union pgt_entry_4kb
{

        struct pde {
                uint32_t flags: 12; /* Bit 0-11  */
                uint32_t base:  20; /* Bit 12-31 */

        } __attribute__ ((packed)) pde;

        struct pte {
                        uint32_t flags: 12; /* Bit 0-11  */
                        uint32_t base:  20; /* Bit 12-31 */

        } __attribute__ ((packed)) pte;
};

/* Page-Translation-Table Entry Fields
   [REF] vol.2, p. 168- */
#define _PTTEF_PRESENT   0
#define _PTTEF_RW        1 /* Read/Write */
#define _PTTEF_US        2 /* User/Supervisor */
#define _PTTEF_ACCESSED  5
#define _PTTEF_DIRTY     6
#define _PTTEF_PAGE_SIZE 7
#define _PTTEF_GLOBAL    8
#define PTTEF_PRESENT    (1 << _PTTEF_PRESENT)
#define PTTEF_RW         (1 << _PTTEF_RW)
#define PTTEF_US         (1 << _PTTEF_US)
#define PTTEF_ACCESSED   (1 << _PTTEF_ACCESSED)
#define PTTEF_DIRTY      (1 << _PTTEF_DIRTY)
#define PTTEF_PAGE_SIZE  (1 << _PTTEF_PAGE_SIZE)
#define PTTEF_GLOBAL     (1 << _PTTEF_GLOBAL)



// Given a physical address,
// return a C pointer the kernel can use to access it.
// This macro does nothing in PIOS because physical memory
// is mapped into the kernel's virtual address space at address 0,
// but this is not the case for many other systems such as JOS or Linux,
// which must do some translation here (usually just adding an offset).
#define mem_ptr(physaddr)	((void*)(physaddr))

// The converse to the above: given a C pointer, return a physical address.
#define mem_phys(ptr)		((uint32_t)(ptr))


// A pageinfo struct holds metadata on how a particular physical page is used.
// On boot we allocate a big array of pageinfo structs, one per physical page.
// This could be a union instead of a struct,
// since only one member is used for a given page state (free, allocated) -
// but that might make debugging a bit more challenging.
typedef struct pageinfo {
	struct pageinfo	*free_next;	// Next page number on free list
	int32_t	refcount;		// Reference count on allocated pages
} pageinfo;


// The pmem module sets up the following globals during mem_init().
extern size_t mem_max;		// Maximum physical address
extern size_t mem_npage;	// Total number of physical memory pages
extern pageinfo *mem_pageinfo;	// Metadata array indexed by page number
//extern static struct pmem_layout pml;

// Convert between pageinfo pointers, page indexes, and physical page addresses
#define mem_phys2pi(phys)	(&mem_pageinfo[(phys)/PAGESIZE])
#define mem_pi2phys(pi)		(((pi)-mem_pageinfo) * PAGESIZE)
#define mem_ptr2pi(ptr)		(mem_phys2pi(mem_phys(ptr)))
#define mem_pi2ptr(pi)		(mem_ptr(mem_pi2phys(pi)))


// Detect available physical memory and initialize the mem_pageinfo array.
void mem_init(const struct multiboot_info *mbi);

// Allocate a physical page and return a pointer to its pageinfo struct.
// Returns NULL if no more physical pages are available.
pageinfo *mem_alloc(void);

// Return a physical page to the free list.
void mem_free(pageinfo *pi);

void mem_incref(pageinfo *pp);
void mem_decref(pageinfo* pp);


struct pmem_layout * get_pmem_layout();

/* [REF] AMD64 manual Vol. 2, pp. 166-167 */

/* For 2-Mbyte page translation (long-mode) */
union pgt_entry_2mb
{
        /* 2-Mbyte PML4E and PDPE */
        struct non_term {
                uint16_t flags: 12; /* Bit 0-11  */
                uint64_t base:  40; /* Bit 12-51 */
                uint16_t avail: 11; /* Bit 52-62 */
                uint16_t nx:    1;  /* Bit 63    */
        } __attribute__ ((packed)) non_term;

        /* 2-Mbyte PDE */
        struct term {
                uint32_t flags: 21; /* Bit 0-20  */
                uint32_t base:  31; /* Bit 21-51 */
                uint16_t avail: 11; /* Bit 52-62 */
                uint16_t nx:    1;  /* Bit 63    */
        } __attribute__ ((packed)) term;
};


extern pageinfo * mem_alloc_contiguous( unsigned long n_pages);
extern unsigned long mem_alloc_one_page();
extern unsigned long alloc_host_pages ( unsigned long nr_pfns, unsigned long pfn_align );
#endif /* !PIOS_KERN_MEM_H */
