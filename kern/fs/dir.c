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
    KERN_DEBUG("CALLING DIR_LOOKUP WITH NAME: %s\n", name );
    uint32_t off, inum;
    struct dirent de;

    if (dp->type != T_DIR)
        KERN_PANIC("dir_lookup not DIR");
    //TODO
    dirent curr_candidate;
    KERN_DEBUG("ATTEMPTING TO SEIZE LOCK IN DIR LOOKUP\n");
    inode_lock(dp);
    KERN_DEBUG("SEIZED LOCK IN DIR LOOKUP\n");
    for(unsigned int i = 0; i < dp->size; i += sizeof(dirent)) { // Maintain offset in the current inode with i
        inode_read(dp, &curr_candidate, i, sizeof(dirent));
        if(dir_namecmp(name, curr_candidate.name) == 0 && curr_candidate.inum != 0) {
            inode_unlock(dp);
            (*poff) = i;
            return inode_get(dp->dev, curr_candidate.inum); 
        }
    }
    KERN_DEBUG("DIR_LOOKUP: FAILED TO FIND THE REQUESTED NAME: %s\n", name );
    inode_unlock(dp);
    return 0;
}

// Write a new directory entry (name, inum) into the directory dp.
int dir_link(struct inode *dp, char *name, uint32_t inum)
{
    /*
    // /* MODIFIED */
    KERN_DEBUG("CALLING DIR_LINK WITH NAME: %s\n", name);
    // TODO: Check that name is not present.
    unsigned int poff; // Spoof an offset variable for the lookup function call.
    inode * existing_entry = dir_lookup(dp, name, poff);
    if(existing_entry != 0) {
        inode_put(existing_entry); // Decrement refcount due to erroneous access.
        return -1;
    }

    // TODO: Look for an empty dirent.
    dirent curr_candidate;
    inode_lock(dp);
    for(unsigned int i = 0; i < dp->size; i += sizeof(dirent)) { // Maintain offset in the current inode with i
        inode_read(dp, &curr_candidate, i, sizeof(dirent));
        if(curr_candidate.inum == 0) {
            
            // Use curr_candidate as the source for an inode write into the current inode with new directory info.
            strncpy(curr_candidate.name, name, DIRSIZ); 
            curr_candidate.inum = inum;
            inode_write(dp, &curr_candidate, i, sizeof(dirent));
            inode_unlock(dp); // No longer need it locked for reading.

            return 0;
        }
    } 
    inode_unlock(dp);
    KERN_PANIC("INODE UNAVAILABLE ON ATTEMPTED DIRECTORY LINK");
    return 1;
}
