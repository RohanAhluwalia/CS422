#include <lib/debug.h>
#include "import.h"

#define PAGESIZE     4096
#define VM_USERLO    0x40000000
#define VM_USERHI    0xF0000000
#define VM_USERLO_PI (VM_USERLO / PAGESIZE)
#define VM_USERHI_PI (VM_USERHI / PAGESIZE)

/**
 * Allocate a physical page.
 *
 * 1. First, implement a naive page allocator that scans the allocation table (AT)
 *    using the functions defined in import.h to find the first unallocated page
 *    with normal permissions.
 *    (Q: Do you have to scan the allocation table from index 0? Recall how you have
 *    initialized the table in pmem_init.)
 *    Then mark the page as allocated in the allocation table and return the page
 *    index of the page found. In the case when there is no available page found,
 *    return 0.
 * 2. Optimize the code using memoization so that you do not have to
 *    scan the allocation table from scratch every time.
 */
static unsigned int MEMO_SEARCH = VM_USERLO_PI;
unsigned int palloc()
{
    // TODO
    for(int i = MEMO_SEARCH; i < VM_USERHI_PI; i++) {
        if(at_is_norm(i) && !at_is_allocated(i)) {
            // Next time, we only search above this address.
            MEMO_SEARCH = i + 1;
            at_set_allocated(i, 1);
            return i;
        }
    }
    return 0;
}

/**
 * Free a physical page.
 *
 * This function marks the page with given index as unallocated
 * in the allocation table.
 *
 * Hint: Simple.
 */
void pfree(unsigned int pfree_index)
{
    // TODO
    if(at_is_norm(pfree_index)) {
        if(pfree_index < MEMO_SEARCH) {
            MEMO_SEARCH = pfree_index;
        } 
        at_set_allocated(pfree_index, 0);
    }
}
