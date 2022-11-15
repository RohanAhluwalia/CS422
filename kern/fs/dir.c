#include <kern/lib/types.h>
#include <kern/lib/debug.h>
#include <kern/lib/string.h>
#include "inode.h"
#include "dir.h"

// Directories

int dir_namecmp(const char *s, const char *t)
{
    return strncmp(s, t, DIRSIZ);
}
typedef struct inode inode;
typedef struct dirent dirent;
/**
 * Look for a directory entry in a directory.
 * If found, set *poff to byte offset of entry.
 */
inode *dir_lookup(inode *dp, char *name, uint32_t * poff)
{
    /* MODIFIED */
    uint32_t off, inum;
    struct dirent de;

    if (dp->type != T_DIR)
        KERN_PANIC("dir_lookup not DIR");

    //TODO
    dirent curr_candidate;
    for(unsigned int i = 0; i < dp->size; i += sizeof(dirent)) { // Maintain offset in the current inode with i
        inode_read(dp, &curr_candidate, i, sizeof(dirent));
        if(dir_namecmp(name, curr_candidate.name) == 0 && curr_candidate.inum != 0) {
            if(*poff != NULL) {
                (*poff) = i;
            }
            return inode_get(dp->dev, curr_candidate.inum); 
        }
    }
    // KERN_DEBUG("DIR_LOOKUP: FAILED TO FIND THE REQUESTED NAME: %s\n", name );
    return 0;
}

// Write a new directory entry (name, inum) into the directory dp.
int dir_link(struct inode *dp, char *name, uint32_t inum)
{
    // /* MODIFIED */
    // KERN_DEBUG("CALLING DIR_LINK WITH NAME: %s\n", name);
    // TODO: Check that name is not present.
    inode * existing_entry = dir_lookup(dp, name, 0);
    if(existing_entry != 0) {
        inode_put(existing_entry); // Decrement refcount due to erroneous access.
        return -1;
    }

    // TODO: Look for an empty dirent.
    dirent curr_candidate;
    // KERN_DEBUG("DIR_LINK: WORKING WITH AN INODE OF SIZE: %ld \n", dp->size);
    for(unsigned int i = 0; i < dp->size; i += sizeof(dirent)) { // Maintain offset in the current inode with i
        inode_read(dp, &curr_candidate, i, sizeof(dirent));
        if(curr_candidate.inum == 0) {
            strncpy(curr_candidate.name, name, DIRSIZ); 
            curr_candidate.inum = inum;
            inode_write(dp, &curr_candidate, i, sizeof(dirent));
            return 0;
        }
    }

    // If we haven't found an empty one among those used, we *attempt* to allocate another one.
    strncpy(curr_candidate.name, name, DIRSIZ); 
    curr_candidate.inum = inum;
    if(inode_write(dp, &curr_candidate, dp->size, sizeof(dirent)) < 0) {
        KERN_PANIC("INODE UNAVAILABLE ON ATTEMPTED DIRECTORY LINK");
        return 1;
    }

    return 0;
}
