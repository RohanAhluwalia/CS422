// File-system system calls.

#include <kern/lib/types.h>
#include <kern/lib/debug.h>
#include <kern/lib/pmap.h>
#include <kern/lib/string.h>
#include <kern/lib/trap.h>
#include <kern/lib/syscall.h>
#include <kern/lib/spinlock.h>
#include <kern/thread/PTCBIntro/export.h>
#include <kern/thread/PCurID/export.h>
#include <kern/trap/TSyscallArg/export.h>

#include "dir.h"
#include "path.h"
#include "file.h"
#include "fcntl.h"
#include "log.h"

/* MODIFIED: Helper Structures/Functions*/
#define FILEBUF_MAX_SIZE 10000
char kernel_buffer[FILEBUF_MAX_SIZE];
spinlock_t kernel_buffer_lk;
spinlock_t* kblk = &kernel_buffer_lk;

void kernel_buffer_lk_init() {
    spinlock_init(kblk);
}
void tf_success(tf_t* tf) {
    syscall_set_errno(tf, E_SUCC);
    syscall_set_retval1(tf, 0);
}

void tf_error(tf_t* tf, enum __error_nr error_opcode) {
    syscall_set_errno(tf, error_opcode);
    syscall_set_retval1(tf, -1);
}

bool buffer_overflows(unsigned int start, unsigned int length) {
    return !(start >= VM_USERLO && start + length <= VM_USERHI);
}

bool exceeds_max_length(unsigned int length, unsigned int max) {
    return (length > max) || (length == 0);
}

/**
 * This function is not a system call handler, but an auxiliary function
 * used by sys_open.
 * Allocate a file descriptor for the given file.
 * You should scan the list of open files for the current thread
 * and find the first file descriptor that is available.
 * Return the found descriptor or -1 if none of them is free.
 */
static int fdalloc(struct file *f)
{
    // TODO
    struct file** files = tcb_get_openfiles(get_curid());
    for(unsigned int i = 0; i < NOFILE; i++) {
        if(files[i] == NULL) {
            tcb_set_openfiles(get_curid(), i, f);
            return i;
        }
    }

    return -1;
}

/**
 * From the file indexed by the given file descriptor, read n bytes and save them
 * into the buffer in the user. As explained in the assignment specification,
 * you should first write to a kernel buffer then copy the data into user buffer
 * with pt_copyout.
 * Return Value: Upon successful completion, read() shall return a non-negative
 * integer indicating the number of bytes actually read. Otherwise, the
 * functions shall return -1 and set errno E_BADF to indicate the error.
 */
void sys_read(tf_t *tf)
{
    // TODO

    // Get initial arguments we need to check bounds for.
    unsigned int pid = get_curid();
    unsigned int fd = syscall_get_arg2(tf);
    unsigned int buf = syscall_get_arg3(tf);
    unsigned int length = syscall_get_arg4(tf);
    //KERN_DEBUG("SYS_READ: ENTERING\n");
    // Check FD validity
    if(fd >= NOFILE) {
        tf_error(tf, E_BADF);
        return;
    }

    // Check bounds on the buffer and n.
    if(buffer_overflows(buf, length)) {
        tf_error(tf, E_INVAL_ADDR);
        return;
    }

    // Check file validity by getting a pointer from user descriptor space.
    struct file* file = tcb_get_openfiles(pid)[fd];
    if(file == NULL || file->type != FD_INODE) {
        tf_error(tf, E_BADF);
        return;
    }

    // Do work
    spinlock_acquire(kblk);
    int read_results = file_read(file, kernel_buffer, length);
    //KERN_DEBUG("SYS_READ: Got passed basic safety checks. (%d)\n", read_results);
    syscall_set_retval1(tf, read_results);
    if(read_results >= 0) {
        pt_copyout(kernel_buffer, pid, buf, length);
        syscall_set_errno(tf, E_SUCC);
    }
    else {
        syscall_set_errno(tf, E_BADF); // Can't call helper (tf_error) bc we need to maintain read results.
    }
    spinlock_release(kblk);

    
}

/**
 * Write n bytes of data in the user's buffer into the file indexed by the file descriptor.
 * You should first copy the data info an in-kernel buffer with pt_copyin and then
 * pass this buffer to appropriate file manipulation function.
 * Upon successful completion, write() shall return the number of bytes actually
 * written to the file associated with f. This number shall never be greater
 * than nbyte. Otherwise, -1 shall be returned and errno E_BADF set to indicate the
 * error.
 */
void sys_write(tf_t *tf)
{
    // TODO

    // Get initial arguments we need to check bounds for.
    unsigned int pid = get_curid();
    unsigned int fd = syscall_get_arg2(tf);
    unsigned int buf = syscall_get_arg3(tf);
    unsigned int length = syscall_get_arg4(tf);
    // KERN_DEBUG("SYS_WRITE: STARTING %ld %ld %ld %ld\n", pid, fd, buf, length);
    // Check FD validity
    if(fd >= NOFILE) {
        tf_error(tf, E_BADF);
        return;
    }

    // Check bounds on the buffer and n.
    if(buffer_overflows(buf, length)) {
        tf_error(tf, E_INVAL_ADDR);
        return;
    }

    // Check file validity by getting a pointer from user descriptor space.
    struct file* file = tcb_get_openfiles(pid)[fd];
    if(file == NULL || file->type != FD_INODE) {
        tf_error(tf, E_BADF);
        return;
    }

    spinlock_acquire(kblk);
    pt_copyin(pid, buf, kernel_buffer, length); // Copy user buffer to kernel buffer
    int write_results = file_write(file, kernel_buffer, length); // Business logic write
    syscall_set_retval1(tf, write_results);
    if(write_results > 0) {
        syscall_set_errno(tf, E_SUCC);
    }
    else {
        syscall_set_errno(tf, E_BADF);
    }
    spinlock_release(kblk);
}

/**
 * Return Value: Upon successful completion, 0 shall be returned; otherwise, -1
 * shall be returned and errno E_BADF set to indicate the error.
 */
void sys_close(tf_t *tf)
{
    // TODO
    unsigned int pid = get_curid();
    unsigned int fd = syscall_get_arg2(tf);

    // Check FD validity
    if(fd >= NOFILE) {
        tf_error(tf, E_BADF);
        return;
    }

    // Check file validity by getting a pointer from user descriptor space.
    struct file* file = tcb_get_openfiles(pid)[fd];
    if(file == NULL || file->ref < 1) {
        tf_error(tf, E_BADF);
        return;
    }

    // Business logic: close the file and void the fd.
    file_close(file);
    tcb_set_openfiles(pid, fd, NULL);
    tf_success(tf);

}

/**
 * Return Value: Upon successful completion, 0 shall be returned. Otherwise, -1
 * shall be returned and errno E_BADF set to indicate the error.
 */
void sys_fstat(tf_t *tf)
{
    // TODO
    unsigned int pid = get_curid();
    unsigned int fd = syscall_get_arg2(tf);
    unsigned int stat_location = syscall_get_arg3(tf);
    if(fd >= NOFILE) {
        tf_error(tf, E_BADF);
        return;
    }

    if(buffer_overflows(stat_location, sizeof(struct file_stat))) {
        tf_error(tf, E_BADF);
        return;
    }

    // Check file validity by getting a pointer from user descriptor space.
    struct file* file = tcb_get_openfiles(pid)[fd];
    if(file == NULL || file->type != FD_INODE) {
        tf_error(tf, E_BADF);
        return;
    }

    // Business Logic
    struct file_stat fs_stat_host;
    int filestat_results = file_stat(file, &fs_stat_host);
    // Check if we have bad results
    if(filestat_results != 0) {
        tf_error(tf, E_BADF);
        return;
    }

    pt_copyout(&filestat_results, pid, stat_location, sizeof(struct file_stat));
    tf_success(tf);


}

/**
 * Create the path new as a link to the same inode as old.
 */
void sys_link(tf_t * tf)
{
    char name[DIRSIZ], new[128], old[128];
    struct inode *dp, *ip;
    unsigned int length1 = syscall_get_arg4(tf) + 1;
    unsigned int length2 = syscall_get_arg5(tf) + 1;

    if(exceeds_max_length(length1, 128) || buffer_overflows(syscall_get_arg2(tf), length1)) {
        tf_error(tf, E_BADF);
        return;
    }
    if(exceeds_max_length(length2, 128) || buffer_overflows(syscall_get_arg3(tf), length2)) {
        tf_error(tf, E_BADF);
        return;
    }

    pt_copyin(get_curid(), syscall_get_arg2(tf), old, length1);
    pt_copyin(get_curid(), syscall_get_arg3(tf), new, length2);

    if ((ip = namei(old)) == 0) {
        syscall_set_errno(tf, E_NEXIST);
        return;
    }

    begin_trans();

    inode_lock(ip);
    if (ip->type == T_DIR) {
        inode_unlockput(ip);
        commit_trans();
        syscall_set_errno(tf, E_DISK_OP);
        return;
    }

    ip->nlink++;
    inode_update(ip);
    inode_unlock(ip);

    if ((dp = nameiparent(new, name)) == 0)
        goto bad;
    inode_lock(dp);
    if (dp->dev != ip->dev || dir_link(dp, name, ip->inum) < 0) {
        inode_unlockput(dp);
        goto bad;
    }
    inode_unlockput(dp);
    inode_put(ip);

    commit_trans();

    syscall_set_errno(tf, E_SUCC);
    return;

bad:
    inode_lock(ip);
    ip->nlink--;
    inode_update(ip);
    inode_unlockput(ip);
    commit_trans();
    syscall_set_errno(tf, E_DISK_OP);
    return;
}

/**
 * Is the directory dp empty except for "." and ".." ?
 */
static int isdirempty(struct inode *dp)
{
    int off;
    struct dirent de;

    for (off = 2 * sizeof(de); off < dp->size; off += sizeof(de)) {
        if (inode_read(dp, (char *) &de, off, sizeof(de)) != sizeof(de))
            KERN_PANIC("isdirempty: readi");
        if (de.inum != 0)
            return 0;
    }
    return 1;
}

void sys_unlink(tf_t *tf)
{
    struct inode *ip, *dp;
    struct dirent de;
    char name[DIRSIZ], path[128];
    uint32_t off;

    unsigned int length = syscall_get_arg3(tf) + 1;
    if(exceeds_max_length(length, 128) || buffer_overflows(syscall_get_arg2(tf), length)) {
        tf_error(tf, E_BADF);
        KERN_DEBUG("FAILING TO START AN UNLINK %ld %ld\n", length, syscall_get_arg2(tf));
        return;
    }

    pt_copyin(get_curid(), syscall_get_arg2(tf), path, length);

    if ((dp = nameiparent(path, name)) == 0) {
        syscall_set_errno(tf, E_DISK_OP);
        return;
    }

    begin_trans();

    inode_lock(dp);

    // Cannot unlink "." or "..".
    if (dir_namecmp(name, ".") == 0 || dir_namecmp(name, "..") == 0)
        goto bad;

    if ((ip = dir_lookup(dp, name, &off)) == 0)
        goto bad;
    inode_lock(ip);

    if (ip->nlink < 1)
        KERN_PANIC("unlink: nlink < 1");
    if (ip->type == T_DIR && !isdirempty(ip)) {
        inode_unlockput(ip);
        goto bad;
    }

    memset(&de, 0, sizeof(de));
    if (inode_write(dp, (char *) &de, off, sizeof(de)) != sizeof(de))
        KERN_PANIC("unlink: writei");
    if (ip->type == T_DIR) {
        dp->nlink--;
        inode_update(dp);
    }
    inode_unlockput(dp);

    ip->nlink--;
    inode_update(ip);
    inode_unlockput(ip);

    commit_trans();

    syscall_set_errno(tf, E_SUCC);
    return;

bad:
    inode_unlockput(dp);
    commit_trans();
    syscall_set_errno(tf, E_DISK_OP);
    return;
}

static struct inode *create(char *path, short type, short major, short minor)
{
    uint32_t off;
    struct inode *ip, *dp;
    char name[DIRSIZ];

    if ((dp = nameiparent(path, name)) == 0)
        return 0;
    inode_lock(dp);

    if ((ip = dir_lookup(dp, name, &off)) != 0) {
        inode_unlockput(dp);
        inode_lock(ip);
        if (type == T_FILE && ip->type == T_FILE)
            return ip;
        inode_unlockput(ip);
        return 0;
    }

    if ((ip = inode_alloc(dp->dev, type)) == 0)
        KERN_PANIC("create: ialloc");

    inode_lock(ip);
    ip->major = major;
    ip->minor = minor;
    ip->nlink = 1;
    inode_update(ip);

    if (type == T_DIR) {  // Create . and .. entries.
        dp->nlink++;      // for ".."
        inode_update(dp);
        // No ip->nlink++ for ".": avoid cyclic ref count.
        if (dir_link(ip, ".", ip->inum) < 0
            || dir_link(ip, "..", dp->inum) < 0)
            KERN_PANIC("create dots");
    }

    if (dir_link(dp, name, ip->inum) < 0)
        KERN_PANIC("create: dir_link");

    inode_unlockput(dp);
    return ip;
}

void sys_open(tf_t *tf)
{
    char path[128];
    int fd, omode;
    struct file *f;
    struct inode *ip;

    unsigned int length = syscall_get_arg4(tf) + 1;
    if(exceeds_max_length(length, 128) || buffer_overflows(syscall_get_arg2(tf), length)) {
        tf_error(tf, E_BADF);
        return;
    }

    pt_copyin(get_curid(), syscall_get_arg2(tf), path, length);
    omode = syscall_get_arg3(tf);

    if (omode & O_CREATE) {
        begin_trans();
        ip = create(path, T_FILE, 0, 0);
        commit_trans();
        if (ip == 0) {
            syscall_set_retval1(tf, -1);
            syscall_set_errno(tf, E_CREATE);
            return;
        }
    } else {
        if ((ip = namei(path)) == 0) {
            syscall_set_retval1(tf, -1);
            syscall_set_errno(tf, E_NEXIST);
            return;
        }
        inode_lock(ip);
        if (ip->type == T_DIR && omode != O_RDONLY) {
            inode_unlockput(ip);
            syscall_set_retval1(tf, -1);
            syscall_set_errno(tf, E_DISK_OP);
            return;
        }
    }

    if ((f = file_alloc()) == 0 || (fd = fdalloc(f)) < 0) {
        if (f)
            file_close(f);
        inode_unlockput(ip);
        syscall_set_retval1(tf, -1);
        syscall_set_errno(tf, E_DISK_OP);
        return;
    }
    inode_unlock(ip);

    f->type = FD_INODE;
    f->ip = ip;
    f->off = 0;
    f->readable = !(omode & O_WRONLY);
    f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
    syscall_set_retval1(tf, fd);
    syscall_set_errno(tf, E_SUCC);
}

void sys_mkdir(tf_t *tf)
{
    char path[128];
    struct inode *ip;

    unsigned int length = syscall_get_arg3(tf) + 1;
    if(exceeds_max_length(length, 128) || buffer_overflows(syscall_get_arg2(tf), length)) {
        tf_error(tf, E_BADF);
        return;
    }

    pt_copyin(get_curid(), syscall_get_arg2(tf), path, length);

    begin_trans();
    if ((ip = (struct inode *) create(path, T_DIR, 0, 0)) == 0) {
        commit_trans();
        syscall_set_errno(tf, E_DISK_OP);
        return;
    }
    inode_unlockput(ip);
    commit_trans();
    syscall_set_errno(tf, E_SUCC);
}

void sys_chdir(tf_t *tf)
{
    char path[128];
    struct inode *ip;
    int pid = get_curid();

    unsigned int length = syscall_get_arg3(tf) + 1;
    if(exceeds_max_length(length, 128) || buffer_overflows(syscall_get_arg2(tf), length)) {
        tf_error(tf, E_BADF);
        return;

    }
    pt_copyin(get_curid(), syscall_get_arg2(tf), path, length);

    if ((ip = namei(path)) == 0) {
        syscall_set_errno(tf, E_DISK_OP);
        return;
    }
    inode_lock(ip);
    if (ip->type != T_DIR) {
        inode_unlockput(ip);
        syscall_set_errno(tf, E_DISK_OP);
        return;
    }
    inode_unlock(ip);
    inode_put(tcb_get_cwd(pid));
    tcb_set_cwd(pid, ip);
    syscall_set_errno(tf, E_SUCC);
}
