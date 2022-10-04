#include <stdio.h>

#define TOTAL_FILES 64 
#define FILE_NAME_LENGTH 15
#define MAX_OPEN_FILE 32
#define FAT_ALLOCATION_BLOCKS 4094

struct superblock
{
    int fatindex;
    int directoryindex;
    int firstdatablockindex;
    int totalblocks;
    int datablocks;
    int fatblocks;
    int blocksize;
    int metablock;

};

struct fatrecord
{   int isFree;
    int nextblock;

};

struct directory
{
   char fileName[FILE_NAME_LENGTH];
   int fileSize;
   int firstDataBlock;
  
};


struct filedescriptor
{
  char fileName[FILE_NAME_LENGTH];
  int  fileDirectoryIndex;
  int  isActive;
  size_t  offset;

};



int make_fs(char *disk_name);
int mount_fs(char *disk_name);
int unmount_fs(char *disk_name);
int fs_open(char *file_name);
int fs_create(char *file_name);
int fs_close(int fildes);
int fs_write(int fildes, void *buf, size_t n_byte);
int fs_read(int fildes, void *buf, size_t n_byte);
int fs_delete(char *name);
int fs_get_filesize(int fildes);
int fs_lseek(int fildes, off_t offset);
int fs_truncate(int fildes, off_t length);






