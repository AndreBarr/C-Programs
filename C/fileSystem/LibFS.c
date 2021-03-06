#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "LibDisk.h"
#include "LibFS.h"

// set to 1 to have detailed debug print-outs and 0 to have none
#define FSDEBUG 0

#if FSDEBUG
#define dprintf printf
#else
#define dprintf noprintf
void noprintf(char* str, ...) {}
#endif

// the file system partitions the disk into five parts:

// 1. the superblock (one sector), which contains a magic number at
// its first four bytes (integer)
#define SUPERBLOCK_START_SECTOR 0

// the magic number chosen for our file system
#define OS_MAGIC 0xdeadbeef

// 2. the inode bitmap (one or more sectors), which indicates whether
// the particular entry in the inode table (#4) is currently in use
#define INODE_BITMAP_START_SECTOR 1

// the total number of bytes and sectors needed for the inode bitmap;
// we use one bit for each inode (whether it's a file or directory) to
// indicate whether the particular inode in the inode table is in use
#define INODE_BITMAP_SIZE ((MAX_FILES+7)/8)
#define INODE_BITMAP_SECTORS ((INODE_BITMAP_SIZE+SECTOR_SIZE-1)/SECTOR_SIZE)

// 3. the sector bitmap (one or more sectors), which indicates whether
// the particular sector in the disk is currently in use
#define SECTOR_BITMAP_START_SECTOR (INODE_BITMAP_START_SECTOR+INODE_BITMAP_SECTORS)

// the total number of bytes and sectors needed for the data block
// bitmap (we call it the sector bitmap); we use one bit for each
// sector of the disk to indicate whether the sector is in use or not
#define SECTOR_BITMAP_SIZE ((TOTAL_SECTORS+7)/8)
#define SECTOR_BITMAP_SECTORS ((SECTOR_BITMAP_SIZE+SECTOR_SIZE-1)/SECTOR_SIZE)

// 4. the inode table (one or more sectors), which contains the inodes
// stored consecutively
#define INODE_TABLE_START_SECTOR (SECTOR_BITMAP_START_SECTOR+SECTOR_BITMAP_SECTORS)

// an inode is used to represent each file or directory; the data
// structure supposedly contains all necessary information about the
// corresponding file or directory
typedef struct _inode {
    int size; // the size of the file or number of directory entries
    int type; // 0 means regular file; 1 means directory
    int data[MAX_SECTORS_PER_FILE]; // indices to sectors containing data blocks
} inode_t;

// the inode structures are stored consecutively and yet they don't
// straddle accross the sector boundaries; that is, there may be
// fragmentation towards the end of each sector used by the inode
// table; each entry of the inode table is an inode structure; there
// are as many entries in the table as the number of files allowed in
// the system; the inode bitmap (#2) indicates whether the entries are
// current in use or not
#define INODES_PER_SECTOR (SECTOR_SIZE/sizeof(inode_t))
#define INODE_TABLE_SECTORS ((MAX_FILES+INODES_PER_SECTOR-1)/INODES_PER_SECTOR)

// 5. the data blocks; all the rest sectors are reserved for data
// blocks for the content of files and directories
#define DATABLOCK_START_SECTOR (INODE_TABLE_START_SECTOR+INODE_TABLE_SECTORS)

// other file related definitions

// max length of a path is 256 bytes (including the ending null)
#define MAX_PATH 256

// max length of a filename is 16 bytes (including the ending null)
#define MAX_NAME 16

// max number of open files is 256
#define MAX_OPEN_FILES 256

// each directory entry represents a file/directory in the parent
// directory, and consists of a file/directory name (less than 16
// bytes) and an integer inode number
typedef struct _dirent {
    char fname[MAX_NAME]; // name of the file
    int inode; // inode of the file
} dirent_t;

// the number of directory entries that can be contained in a sector
#define DIRENTS_PER_SECTOR (SECTOR_SIZE/sizeof(dirent_t))

// global errno value here
int osErrno;

// the name of the disk backstore file (with which the file system is booted)
static char bs_filename[1024];

/* the following functions are internal helper functions */

// check magic number in the superblock; return 1 if OK, and 0 if not
static int check_magic()
{
    char buf[SECTOR_SIZE];
    if(Disk_Read(SUPERBLOCK_START_SECTOR, buf) < 0)
        return 0;
    if(*(int*)buf == OS_MAGIC) return 1;
    else return 0;
}

// initialize a bitmap with 'num' sectors starting from 'start'
// sector; all bits should be set to zero except that the first
// 'nbits' number of bits are set to one
static void bitmap_init(int start, int num, int nbits)
{
    /* YOUR CODE */
    int numBytes = nbits / 8;
    int remainingBits = nbits % 8;
    int fullOneSectors = numBytes / SECTOR_SIZE;
    
    for (int i = start; i < start + num; i++) {
        char bitmap_buffer[SECTOR_SIZE];
        if (fullOneSectors > 0) {
            for (int j = 0; j < SECTOR_SIZE; j++) {
                bitmap_buffer[j] = (unsigned char)255;
            }
            
            numBytes -= SECTOR_SIZE;
            fullOneSectors--;
        }
        else if (numBytes > 0) {
            int j;
            for (j = 0; j < numBytes; j++) {
                bitmap_buffer[j] = (unsigned char)255;
            }
            numBytes = 0;
            int specialByte = 0;
            for (int k = 0; k < 7; k++) {
                if (remainingBits > 0) {
                    specialByte |= 1 << remainingBits;
                    remainingBits--;
                }
                else {
                    specialByte <<= 1;
                }
            }
            
            bitmap_buffer[j] = (unsigned char)specialByte;
            j++;
            
            for (; j < SECTOR_SIZE; j++) {
                bitmap_buffer[j] = (unsigned char)0;
            }
        } else {
            for (int j = 0; j < SECTOR_SIZE; j++) {
                bitmap_buffer[j] = (unsigned char)0;
            }
        }
        
        Disk_Write(i, bitmap_buffer);
    }
}

// set the first unused bit from a bitmap of 'nbits' bits (flip the
// first zero appeared in the bitmap to one) and return its location;
// return -1 if the bitmap is already full (no more zeros)
static int bitmap_first_unused(int start, int num, int nbits)
{
    /* YOUR CODE */
    int location = -1;
    int locationFound = 0;
    
    int i = start;
    
    do {
        char currentSector[SECTOR_SIZE];
        Disk_Read(i, currentSector);
        int j = 0;
        do {
            char currentByte = currentSector[j];
            if (currentByte != (char)255) {
                locationFound = 1;
                int inByteLocation = 0;
                unsigned char byteMask = (unsigned char)128;
                char temp = currentByte;
                
                do {
                    byteMask &= temp;
                    temp <<= 1;
                    inByteLocation++;
                } while (byteMask == (unsigned char)128);
                
                byteMask = (unsigned char)1;
                byteMask <<= (8 - inByteLocation);
                
                currentByte |= byteMask;
                currentSector[j] = currentByte;
                
                Disk_Write(i, currentSector);
                
                location = ((i - start)*SECTOR_SIZE * 8) + (j * 8) + inByteLocation;
            }
            j++;
        } while (j < SECTOR_SIZE && !locationFound);
        i++;
    } while (i < (start + num) && !locationFound);
    
    return location;
}

// reset the i-th bit of a bitmap with 'num' sectors starting from
// 'start' sector; return 0 if successful, -1 otherwise
static int bitmap_reset(int start, int num, int ibit)
{
    /* YOUR CODE */
    int error = -1;
    
    if (ibit <= (num * SECTOR_SIZE * 8)) {
        int whichByte = (ibit - 1) / 8;
        int whichSector = whichByte / SECTOR_SIZE;
        whichByte -= (whichSector * 512);
        int inByteLocation = (whichByte % 8) + 1;
        
        unsigned char byteMask = ((unsigned char)255) ^ 1<<(8 - inByteLocation);
        
        char sector[SECTOR_SIZE];
        Disk_Read(whichSector, sector);
        
        sector[whichByte] &= byteMask;
        
        Disk_Write(whichSector, sector);
        
        error = 0;
    }
    
    return error;
}

// return 1 if the file name is illegal; otherwise, return 0; legal
// characters for a file name include letters (case sensitive),
// numbers, dots, dashes, and underscores; and a legal file name
// should not be more than MAX_NAME-1 in length
static int illegal_filename(char* name)
{
    /* YOUR CODE */
    int error = 1;
    int valid = -1;
    const char * illegalChars = " !@#$\%^&*()+={}[]\\|;:\'\",<>/?`~";
    
    if (strlen(name) <= (MAX_NAME - 1)) {
        char *token = strtok(name, illegalChars);
        while (token != NULL) {
            token = strtok(NULL, illegalChars);
            valid++;
        }
    }
    
    if (valid == 0) {
        error = valid;
    }
    
    return error;
}

// return the child inode of the given file name 'fname' from the
// parent inode; the parent inode is currently stored in the segment
// of inode table in the cache (we cache only one disk sector for
// this); once found, both cached_inode_sector and cached_inode_buffer
// may be updated to point to the segment of inode table containing
// the child inode; the function returns -1 if no such file is found;
// it returns -2 is something else is wrong (such as parent is not
// directory, or there's read error, etc.)
static int find_child_inode(int parent_inode, char* fname,
                            int *cached_inode_sector, char* cached_inode_buffer)
{
    int cached_start_entry = ((*cached_inode_sector)-INODE_TABLE_START_SECTOR)*INODES_PER_SECTOR;
    int offset = parent_inode-cached_start_entry;
    assert(0 <= offset && offset < INODES_PER_SECTOR);
    inode_t* parent = (inode_t*)(cached_inode_buffer+offset*sizeof(inode_t));
    dprintf("... load parent inode: %d (size=%d, type=%d)\n",
            parent_inode, parent->size, parent->type);
    if(parent->type != 1) {
        dprintf("... parent not a directory\n");
        return -2;
    }
    
    int nentries = parent->size; // remaining number of directory entries
    int idx = 0;
    while(nentries > 0) {
        char buf[SECTOR_SIZE]; // cached content of directory entries
        if(Disk_Read(parent->data[idx], buf) < 0) return -2;
        for(int i=0; i<DIRENTS_PER_SECTOR; i++) {
            if(i>nentries) break;
            if(!strcmp(((dirent_t*)buf)[i].fname, fname)) {
                // found the file/directory; update inode cache
                int child_inode = ((dirent_t*)buf)[i].inode;
                dprintf("... found child_inode=%d\n", child_inode);
                int sector = INODE_TABLE_START_SECTOR+child_inode/INODES_PER_SECTOR;
                if(sector != (*cached_inode_sector)) {
                    *cached_inode_sector = sector;
                    if(Disk_Read(sector, cached_inode_buffer) < 0) return -2;
                    dprintf("... load inode table for child\n");
                }
                return child_inode;
            }
        }
        idx++; nentries -= DIRENTS_PER_SECTOR;
    }
    dprintf("... could not find child inode\n");
    return -1; // not found
}

// follow the absolute path; if successful, return the inode of the
// parent directory immediately before the last file/directory in the
// path; for example, for '/a/b/c/d.txt', the parent is '/a/b/c' and
// the child is 'd.txt'; the child's inode is returned through the
// parameter 'last_inode' and its file name is returned through the
// parameter 'last_fname' (both are references); it's possible that
// the last file/directory is not in its parent directory, in which
// case, 'last_inode' points to -1; if the function returns -1, it
// means that we cannot follow the path
static int follow_path(char* path, int* last_inode, char* last_fname)
{
    if(!path) {
        dprintf("... invalid path\n");
        return -1;
    }
    if(path[0] != '/') {
        dprintf("... '%s' not absolute path\n", path);
        return -1;
    }
    
    // make a copy of the path (skip leading '/'); this is necessary
    // since the path is going to be modified by strsep()
    char pathstore[MAX_PATH];
    strncpy(pathstore, path+1, MAX_PATH-1);
    pathstore[MAX_PATH-1] = '\0'; // for safety
    char* lpath = pathstore;
    
    int parent_inode = -1, child_inode = 0; // start from root
    // cache the disk sector containing the root inode
    int cached_sector = INODE_TABLE_START_SECTOR;
    char cached_buffer[SECTOR_SIZE];
    if(Disk_Read(cached_sector, cached_buffer) < 0) return -1;
    dprintf("... load inode table for root from disk sector %d\n", cached_sector);
    
    // for each file/directory name separated by '/'
    char* token;
    while((token = strsep(&lpath, "/")) != NULL) {
        dprintf("... process token: '%s'\n", token);
        if(*token == '\0') continue; // multiple '/' ignored
        if(illegal_filename(token)) {
            dprintf("... illegal file name: '%s'\n", token);
            return -1;
        }
        if(child_inode < 0) {
            // regardless whether child_inode was not found previously, or
            // there was issues related to the parent (say, not a
            // directory), or there was a read error, we abort
            dprintf("... parent inode can't be established\n");
            return -1;
        }
        parent_inode = child_inode;
        child_inode = find_child_inode(parent_inode, token,
                                       &cached_sector, cached_buffer);
        if(last_fname) strcpy(last_fname, token);
    }
    if(child_inode < -1) return -1; // if there was error, abort
    else {
        // there was no error, several possibilities:
        // 1) '/': parent = -1, child = 0
        // 2) '/valid-dirs.../last-valid-dir/not-found': parent=last-valid-dir, child=-1
        // 3) '/valid-dirs.../last-valid-dir/found: parent=last-valid-dir, child=found
        // in the first case, we set parent=child=0 as special case
        if(parent_inode==-1 && child_inode==0) parent_inode = 0;
        dprintf("... found parent_inode=%d, child_inode=%d\n", parent_inode, child_inode);
        *last_inode = child_inode;
        return parent_inode;
    }
}

// add a new file or directory (determined by 'type') of given name
// 'file' under parent directory represented by 'parent_inode'
int add_inode(int type, int parent_inode, char* file)
{
    // get a new inode for child
    int child_inode = bitmap_first_unused(INODE_BITMAP_START_SECTOR, INODE_BITMAP_SECTORS, INODE_BITMAP_SIZE);
    if(child_inode < 0) {
        dprintf("... error: inode table is full\n");
        return -1;
    }
    dprintf("... new child inode %d\n", child_inode);
    
    // load the disk sector containing the child inode
    int inode_sector = INODE_TABLE_START_SECTOR+child_inode/INODES_PER_SECTOR;
    char inode_buffer[SECTOR_SIZE];
    if(Disk_Read(inode_sector, inode_buffer) < 0) return -1;
    dprintf("... load inode table for child inode from disk sector %d\n", inode_sector);
    
    // get the child inode
    int inode_start_entry = (inode_sector-INODE_TABLE_START_SECTOR)*INODES_PER_SECTOR;
    int offset = child_inode-inode_start_entry;
    assert(0 <= offset && offset < INODES_PER_SECTOR);
    inode_t* child = (inode_t*)(inode_buffer+offset*sizeof(inode_t));
    
    // update the new child inode and write to disk
    memset(child, 0, sizeof(inode_t));
    child->type = type;
    if(Disk_Write(inode_sector, inode_buffer) < 0) return -1;
    dprintf("... update child inode %d (size=%d, type=%d), update disk sector %d\n",
            child_inode, child->size, child->type, inode_sector);
    
    // get the disk sector containing the parent inode
    inode_sector = INODE_TABLE_START_SECTOR+parent_inode/INODES_PER_SECTOR;
    if(Disk_Read(inode_sector, inode_buffer) < 0) return -1;
    dprintf("... load inode table for parent inode %d from disk sector %d\n",
            parent_inode, inode_sector);
    
    // get the parent inode
    inode_start_entry = (inode_sector-INODE_TABLE_START_SECTOR)*INODES_PER_SECTOR;
    offset = parent_inode-inode_start_entry;
    assert(0 <= offset && offset < INODES_PER_SECTOR);
    inode_t* parent = (inode_t*)(inode_buffer+offset*sizeof(inode_t));
    dprintf("... get parent inode %d (size=%d, type=%d)\n",
            parent_inode, parent->size, parent->type);
    
    // get the dirent sector
    if(parent->type != 1) {
        dprintf("... error: parent inode is not directory\n");
        return -2; // parent not directory
    }
    int group = parent->size/DIRENTS_PER_SECTOR;
    char dirent_buffer[SECTOR_SIZE];
    if(group*DIRENTS_PER_SECTOR == parent->size) {
        // new disk sector is needed
        int newsec = bitmap_first_unused(SECTOR_BITMAP_START_SECTOR, SECTOR_BITMAP_SECTORS, SECTOR_BITMAP_SIZE);
        if(newsec < 0) {
            dprintf("... error: disk is full\n");
            return -1;
        }
        parent->data[group] = newsec;
        memset(dirent_buffer, 0, SECTOR_SIZE);
        dprintf("... new disk sector %d for dirent group %d\n", newsec, group);
    } else {
        if(Disk_Read(parent->data[group], dirent_buffer) < 0)
            return -1;
        dprintf("... load disk sector %d for dirent group %d\n", parent->data[group], group);
    }
    
    // add the dirent and write to disk
    int start_entry = group*DIRENTS_PER_SECTOR;
    offset = parent->size-start_entry;
    dirent_t* dirent = (dirent_t*)(dirent_buffer+offset*sizeof(dirent_t));
    strncpy(dirent->fname, file, MAX_NAME);
    dirent->inode = child_inode;
    if(Disk_Write(parent->data[group], dirent_buffer) < 0) return -1;
    dprintf("... append dirent %d (name='%s', inode=%d) to group %d, update disk sector %d\n",
            parent->size, dirent->fname, dirent->inode, group, parent->data[group]);
    
    // update parent inode and write to disk
    parent->size++;
    if(Disk_Write(inode_sector, inode_buffer) < 0) return -1;
    dprintf("... update parent inode on disk sector %d\n", inode_sector);
    
    return 0;
}

// used by both File_Create() and Dir_Create(); type=0 is file, type=1
// is directory
int create_file_or_directory(int type, char* pathname)
{
    int child_inode;
    char last_fname[MAX_NAME];
    int parent_inode = follow_path(pathname, &child_inode, last_fname);
    if(parent_inode >= 0) {
        if(child_inode >= 0) {
            dprintf("... file/directory '%s' already exists, failed to create\n", pathname);
            osErrno = E_CREATE;
            return -1;
        } else {
            if(add_inode(type, parent_inode, last_fname) >= 0) {
                dprintf("... successfully created file/directory: '%s'\n", pathname);
                return 0;
            } else {
                dprintf("... error: something wrong with adding child inode\n");
                osErrno = E_CREATE;
                return -1;
            }
        }
    } else {
        dprintf("... error: something wrong with the file/path: '%s'\n", pathname);
        osErrno = E_CREATE;
        return -1;
    }
}


inode_t* get_inode(int an_i_node) {
    int inode_sector = INODE_TABLE_START_SECTOR + an_i_node / INODES_PER_SECTOR;
    char inode_buffer[SECTOR_SIZE];
    Disk_Read(inode_sector, inode_buffer);
    
    int inode_start_entry = (inode_sector - INODE_TABLE_START_SECTOR)*INODES_PER_SECTOR;
    int offset = an_i_node - inode_start_entry;
    inode_t* the_i_node = (inode_t*)(inode_buffer + offset * sizeof(inode_t));
    
    return the_i_node;
}

void removeChild(int parent_inode, int child_inode) {
    inode_t* parent = get_inode(parent_inode);
    
    for (int i = 0; i < MAX_SECTORS_PER_FILE; i++) {
        int currentSector = parent->data[i];
        char currentSectorBuffer[SECTOR_SIZE];
        
        Disk_Read(currentSector, currentSectorBuffer);
        
        int maxEntryPerSector = SECTOR_SIZE / sizeof(dirent_t);
        
        int j = 0;
        int childFound = 0;
        while (!childFound && j < maxEntryPerSector) {
            dirent_t * currentEntry = (dirent_t *)(currentSectorBuffer + (j * sizeof(dirent_t)));
            
            if (currentEntry->inode == child_inode) {
                memset(currentEntry, 0, sizeof(dirent_t));
                Disk_Write(currentSector, currentSectorBuffer);
                childFound = 1;
            }
            
            j++;
        }
    }
    
    return;
}

// remove the child from parent; the function is called by both
// File_Unlink() and Dir_Unlink(); the function returns 0 if success,
// -1 if general error, -2 if directory not empty, -3 if wrong type
int remove_inode(int type, int parent_inode, int child_inode)
{
    /* YOUR CODE */
    int error = -2;
    
    if (type == 0) { // File
        inode_t* child = get_inode(child_inode);
        char sectorBuffer[SECTOR_SIZE];
        
        for (int i = 0; i < MAX_SECTORS_PER_FILE; i++) {
            int sector = (unsigned char)child->data[i];
            
            if (sector != 0) {
                memset(buffer, 0, SECTOR_SIZE);
                Disk_Write(sector, buffer);
                bitmap_reset(SECTOR_BITMAP_START_SECTOR, SECTOR_BITMAP_SECTORS, sector + 1);
            }
        }
        bitmap_reset(INODE_BITMAP_START_SECTOR, INODE_BITMAP_SECTORS, child_inode + 1);
        removeChild(parent_inode, child_inode);
        error = 0;
    } else if (type == 1) { // Directory
        removeChild(parent_inode, child_inode);
        error = 0;
    } else {
        error = -3;
    }
    return error;
}

// representing an open file
typedef struct _open_file {
    int inode; // pointing to the inode of the file (0 means entry not used)
    int size;  // file size cached here for convenience
    int pos;   // read/write position
} open_file_t;
static open_file_t open_files[MAX_OPEN_FILES];

// return true if the file pointed to by inode has already been open
int is_file_open(int inode)
{
    for(int i=0; i<MAX_OPEN_FILES; i++) {
        if(open_files[i].inode == inode)
            return 1;
    }
    return 0;
}

// return a new file descriptor not used; -1 if full
int new_file_fd()
{
    for(int i=0; i<MAX_OPEN_FILES; i++) {
        if(open_files[i].inode <= 0)
            return i;
    }
    return -1;
}

/* end of internal helper functions, start of API functions */

int FS_Boot(char* backstore_fname)
{
    dprintf("FS_Boot('%s'):\n", backstore_fname);
    // initialize a new disk (this is a simulated disk)
    if(Disk_Init() < 0) {
        dprintf("... disk init failed\n");
        osErrno = E_GENERAL;
        return -1;
    }
    dprintf("... disk initialized\n");
    
    // we should copy the filename down; if not, the user may change the
    // content pointed to by 'backstore_fname' after calling this function
    strncpy(bs_filename, backstore_fname, 1024);
    bs_filename[1023] = '\0'; // for safety
    
    // we first try to load disk from this file
    if(Disk_Load(bs_filename) < 0) {
        dprintf("... load disk from file '%s' failed\n", bs_filename);
        
        // if we can't open the file; it means the file does not exist, we
        // need to create a new file system on disk
        if(diskErrno == E_OPENING_FILE) {
            dprintf("... couldn't open file, create new file system\n");
            
            // format superblock
            char buf[SECTOR_SIZE];
            memset(buf, 0, SECTOR_SIZE);
            *(int*)buf = OS_MAGIC;
            if(Disk_Write(SUPERBLOCK_START_SECTOR, buf) < 0) {
                dprintf("... failed to format superblock\n");
                osErrno = E_GENERAL;
                return -1;
            }
            dprintf("... formatted superblock (sector %d)\n", SUPERBLOCK_START_SECTOR);
            
            // format inode bitmap (reserve the first inode to root)
            bitmap_init(INODE_BITMAP_START_SECTOR, INODE_BITMAP_SECTORS, 1);
            dprintf("... formatted inode bitmap (start=%d, num=%d)\n",
                    (int)INODE_BITMAP_START_SECTOR, (int)INODE_BITMAP_SECTORS);
            
            // format sector bitmap (reserve the first few sectors to
            // superblock, inode bitmap, sector bitmap, and inode table)
            bitmap_init(SECTOR_BITMAP_START_SECTOR, SECTOR_BITMAP_SECTORS,
                        DATABLOCK_START_SECTOR);
            dprintf("... formatted sector bitmap (start=%d, num=%d)\n",
                    (int)SECTOR_BITMAP_START_SECTOR, (int)SECTOR_BITMAP_SECTORS);
            
            // format inode tables
            for(int i=0; i<INODE_TABLE_SECTORS; i++) {
                memset(buf, 0, SECTOR_SIZE);
                if(i==0) {
                    // the first inode table entry is the root directory
                    ((inode_t*)buf)->size = 0;
                    ((inode_t*)buf)->type = 1;
                }
                if(Disk_Write(INODE_TABLE_START_SECTOR+i, buf) < 0) {
                    dprintf("... failed to format inode table\n");
                    osErrno = E_GENERAL;
                    return -1;
                }
            }
            dprintf("... formatted inode table (start=%d, num=%d)\n",
                    (int)INODE_TABLE_START_SECTOR, (int)INODE_TABLE_SECTORS);
            
            // we need to synchronize the disk to the backstore file (so
            // that we don't lose the formatted disk)
            if(Disk_Save(bs_filename) < 0) {
                // if can't write to file, something's wrong with the backstore
                dprintf("... failed to save disk to file '%s'\n", bs_filename);
                osErrno = E_GENERAL;
                return -1;
            } else {
                // everything's good now, boot is successful
                dprintf("... successfully formatted disk, boot successful\n");
                memset(open_files, 0, MAX_OPEN_FILES*sizeof(open_file_t));
                return 0;
            }
        } else {
            // something wrong loading the file: invalid param or error reading
            dprintf("... couldn't read file '%s', boot failed\n", bs_filename);
            osErrno = E_GENERAL;
            return -1;
        }
    } else {
        dprintf("... load disk from file '%s' successful\n", bs_filename);
        
        // we successfully loaded the disk, we need to do two more checks,
        // first the file size must be exactly the size as expected (thiis
        // supposedly should be folded in Disk_Load(); and it's not)
        int sz = 0;
        FILE* f = fopen(bs_filename, "r");
        if(f) {
            fseek(f, 0, SEEK_END);
            sz = ftell(f);
            fclose(f);
        }
        if(sz != SECTOR_SIZE*TOTAL_SECTORS) {
            dprintf("... check size of file '%s' failed\n", bs_filename);
            osErrno = E_GENERAL;
            return -1;
        }
        dprintf("... check size of file '%s' successful\n", bs_filename);
        
        // check magic
        if(check_magic()) {
            // everything's good by now, boot is successful
            dprintf("... check magic successful\n");
            memset(open_files, 0, MAX_OPEN_FILES*sizeof(open_file_t));
            return 0;
        } else {
            // mismatched magic number
            dprintf("... check magic failed, boot failed\n");
            osErrno = E_GENERAL;
            return -1;
        }
    }
}

int FS_Sync()
{
    if(Disk_Save(bs_filename) < 0) {
        // if can't write to file, something's wrong with the backstore
        dprintf("FS_Sync():\n... failed to save disk to file '%s'\n", bs_filename);
        osErrno = E_GENERAL;
        return -1;
    } else {
        // everything's good now, sync is successful
        dprintf("FS_Sync():\n... successfully saved disk to file '%s'\n", bs_filename);
        return 0;
    }
}

int File_Create(char* file)
{
    dprintf("File_Create('%s'):\n", file);
    return create_file_or_directory(0, file);
}

int File_Unlink(char* file)
{
    /* YOUR CODE */
    char file_name[MAX_NAME];
    int child_inode;
    int parent_inode = follow_path(file, &child_inode, file_name); // Finding the Parent
    int success = 0;
    int error = -1;
    
    if (child_inode < 1) { // Check if File does not Exist
        osErrno = E_NO_SUCH_FILE;
        return error;
    }
    
    if (is_file_open(child_inode) == 1) { // Check if File is in Use
        osErrno = E_FILE_IN_USE;
        return error;
    }
    
    if (remove_inode(0, parent_inode, child_inode) >= 0) { // Check if File can be Deleted
        return success;
    }
    
    return error;
}

int File_Open(char* file)
{
    dprintf("File_Open('%s'):\n", file);
    int fd = new_file_fd();
    if(fd < 0) {
        dprintf("... max open files reached\n");
        osErrno = E_TOO_MANY_OPEN_FILES;
        return -1;
    }
    
    int child_inode;
    follow_path(file, &child_inode, NULL);
    if(child_inode >= 0) { // child is the one
        // load the disk sector containing the inode
        int inode_sector = INODE_TABLE_START_SECTOR+child_inode/INODES_PER_SECTOR;
        char inode_buffer[SECTOR_SIZE];
        if(Disk_Read(inode_sector, inode_buffer) < 0) { osErrno = E_GENERAL; return -1; }
        dprintf("... load inode table for inode from disk sector %d\n", inode_sector);
        
        // get the inode
        int inode_start_entry = (inode_sector-INODE_TABLE_START_SECTOR)*INODES_PER_SECTOR;
        int offset = child_inode-inode_start_entry;
        assert(0 <= offset && offset < INODES_PER_SECTOR);
        inode_t* child = (inode_t*)(inode_buffer+offset*sizeof(inode_t));
        dprintf("... inode %d (size=%d, type=%d)\n",
                child_inode, child->size, child->type);
        
        if(child->type != 0) {
            dprintf("... error: '%s' is not a file\n", file);
            osErrno = E_GENERAL;
            return -1;
        }
        
        // initialize open file entry and return its index
        open_files[fd].inode = child_inode;
        open_files[fd].size = child->size;
        open_files[fd].pos = 0;
        return fd;
    } else {
        dprintf("... file '%s' is not found\n", file);
        osErrno = E_NO_SUCH_FILE;
        return -1;
    }
}

int File_Read(int fd, void* buffer, int size)
{
    /* YOUR CODE */
    int readBytes = 0;
    
    // Check for Errors
    if (open_files[fd] == 0) {
        dprintf("File Not Open! :(");
        osErrno = E_BAD_FD;
        return -1;
    }
    
    // Load the Disk Sector that Contains the inode
    
    
    return -1;
}

int File_Write(int fd, void* buffer, int size)
{
    /* YOUR CODE */
    int initialPos = open_files[fd].pos;
    int start = open_files[fd].pos / SECTOR_SIZE;
    int end = start + ((size+open_files[fd].pos+SECTOR_SIZE-1)/SECTOR_SIZE);
    int remainingBytes = size;
    
    // First Check For Errors
    if (fd < 0 && fd > MAX_OPEN_FILES) { // Not Enough Space on the Disk
        osErrno = E_NO_SPACE;
        return -1;
    }
    
    if(open_files[fd].inode < 1) { // File is not Open
        osErrno = E_BAD_FD;
        return -1;
    }
    
    if (end > MAX_SECTORS_PER_FILE) { // File Too Big
        osErrno = E_FILE_TOO_BIG;
        return -1;
    }
    
    inode_t* inode = get_inode(open_files[fd].inode);
    int i; // Making sure it works on OCELOT
    
    for (i = start; i < end; i++) { // Write Byte by Byte
        int sector = inode->data[i];
        char diskBuffer[SECTOR_SIZE];
        char dataBuffer[SECTOR_SIZE];
        
        if (sector == 0) { // Get Location Where Info Should be Stored
            sector = bitmap_first_unused(SECTOR_BITMAP_START_SECTOR, SECTOR_BITMAP_SECTORS, SECTOR_BITMAP_SIZE);
        }
        
        if (open_files[fd].pos != 0) {
            Disk_Read(sector, diskBuffer); // Reading Data from Sector
            memcpy(diskBuffer, (diskBuffer+open_files[fd].size), SECTOR_SIZE-open_files[fd].size);
            Disk_Write(sector, diskBuffer); // Writing Data to Sector
            remainingBytes -= SECTOR_SIZE-open_files[fd].size;
            open_files[fd].pos = 0;
        } else {
            // Check if all remaining bits fit in one sector
            if (remainingBytes >= SECTOR_SIZE) { // Buffer Will get as much as Possible
                memcpy(diskBuffer, (dataBuffer + i*SECTOR_SIZE), SECTOR_SIZE);
                remainingBytes -= SECTOR_SIZE;
            } else { // Buffer Will Get Everything Left
                memcpy(diskBuffer, (dataBuffer + i*SECTOR_SIZE), remainingBytes);
                remainingBytes = 0;
            }
            Disk_Write(sector, diskBuffer); // Writing Data to Sector
        }
    }
    open_files[fd].pos = initialPos+size; // Moving End File Pointer
    return size; // Returning Amount of Bytes Added
}

int File_Seek(int fd, int offset)
{
    /* YOUR CODE */
    // First Check For Errors
    if (is_file_open(open_files[fd].inode) != 1) { // File Not Open
        osErrno = E_BAD_FD;
        return -1;
    }
    
    if (open_files[fd].size < 0 || open_files[fd].size < offset) {
        osErrno = E_SEEK_OUT_OF_BOUNDS;
        return -1
    }
    
    open_files[fd].pos = offset; // Setting the New Location of The Pointer
    return open_files[fd].pos;
}

int File_Close(int fd)
{
    dprintf("File_Close(%d):\n", fd);
    if(0 > fd || fd > MAX_OPEN_FILES) {
        dprintf("... fd=%d out of bound\n", fd);
        osErrno = E_BAD_FD;
        return -1;
    }
    if(open_files[fd].inode <= 0) {
        dprintf("... fd=%d not an open file\n", fd);
        osErrno = E_BAD_FD;
        return -1;
    }
    
    dprintf("... file closed successfully\n");
    open_files[fd].inode = 0;
    return 0;
}

int Dir_Create(char* path)
{
    dprintf("Dir_Create('%s'):\n", path);
    return create_file_or_directory(1, path);
}

// Helper Function to see if path leads to Directory or leads to a File
int File_Or_Dir(char* pathname) {
    char file_name[MAX_NAME];
    int sign;
    int result = follow_path(pathname, &sign, file_name); // Finding the sign
    if (result == -1) {
        return -1;
    } else {
        if (sign == -1) { // Token is Invalid
            return -1
        }
        
        inode_t* inode = get_inode(sign); // Getting the inode from the token
        return inode->type; // Returning the Type
    }
}

// This Function Unlinks the Directory
int Dir_Unlink(char* path)
{
    /* YOUR CODE */
    int success = 0;
    int error = -1;
    if (!strcmp(path, "/") != 0) { // Dir Does Not Exist
        osErrno = E_NO_SUCH_DIR;
        return error;
    } else if (strcmp(path, "/") == 0) { // Root Dir Cannot be Unlinked
        osErrno = E_ROOT_DIR;
        return error;
    } else if (File_Or_Dir(path) == 1) { // Checking if Path is a Directory
        char file_name[MAX_NAME];
        int token;
        int parent_inode = follow_path(path, &token, file_name);
        
        inode_t* inode = get_inode(token); // Getting the Inode of Token
        if (inode->size > 0) { // Checking if the directory still contains files
            osErrno = E_DIR_NOT_EMPTY;
            return error;
        } else if (remove_inode(1, parent_inode, token) >= 0) { // Everything Checks. Now Removing the Directory
            return success;
        }
    }
    return error;
}

// This Function Outputs the size of the Directory
int Dir_Size(char* path)
{
    /* YOUR CODE */
    // Checking Path Type
    if (File_Or_Dir(path) == 1) { // Checking if Path is a Directory
        char file_name[MAX_NAME];
        int token;
        follow_path(path, &token, file_name); // Find the location of Token
        inode_t* directory_inode = get_inode(token); // Get the inode of Token
        
        int byteCounter = 0;
        int i; // initialize before for loop in order to compile with OCELOT
        
        for (i = 0; i < 30; i++) { // For all 30 allocated data blocks per file
            int sector = (unsigned char) directory_inode->data[i];
            char sectorBuffer[SECTOR_SIZE];
            Disk_Read(sector, sectorBuffer); // Saving Data onto SectorBuffer
            
            int j;
            for (j = 0; j < 25; j++) {
                dirent_t* entry (dirent_t*)(sectorBuffer + (j*20)); // Multiply by 20 because each entry contains 16 byte names plus the 4 byte integer inode numbers
                if (entry->inode > 0) {
                    byteCounter += 20; // Because Each Entry is 20 Bytes
                }
            }
        }
        return byteCounter;
    }
    
    return 0;
}

// This Function Reads the contents of the Directory
int Dir_Read(char* path, void* buffer, int size)
{
    /* YOUR CODE */
    int byteCounter = 0;
    if (File_Or_Dir(path) == 1) {
        int sign;
        char filename[MAX_NAME];
        follow_path(path, &sign, filename);
        
        int directorySize = Dir_Size(path);
        
        // Check if Size fits Directory Size
        if (directorySize > size) {
            osErrno = E_BUFFER_TOO_SMALL;
            return -1;
        }
        
        inode_t* inode = get_inode(token);
        if (inode->type == 1) {
            int j;
            for (j = 0; j < 30; j++) {
                int sector = (unsigned char) inode->data[i];
                char sectorBuffer[SECTOR_SIZE];
                Disk_Read(sector, sectorBuffer);
            }
        }
        return byteCounter;
    }
    
    return -1;
}

