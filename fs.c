

#include <stdio.h>
#include "fs.h"
#include "disk.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>



struct superblock *sb;
struct fatrecord *fattable;
struct directory *directory;
struct filedescriptor *fd;

struct superblock *read_sb;
struct fatrecord *read_fattable;
struct directory *read_directory;
int isFileDescriptorExist = 0;
int alreadyOpen = 0;




int make_fs(char *disk_name)
{
    if(make_disk(disk_name)<0)
    {
	return -1;
    }
    if(open_disk(disk_name)<0)
    {
	return -1;
    }
    alreadyOpen = 1;
    sb = malloc(BLOCK_SIZE);
    sb->fatindex= 1;
    sb->directoryindex = 4095;
    sb->firstdatablockindex = 4096;
    sb->totalblocks = 8192;
    sb->datablocks = 4096;
    sb->blocksize = 4096;
    sb->metablock = 4096;
    sb->fatblocks = 4096;
    
    
    fattable = malloc(FAT_ALLOCATION_BLOCKS*BLOCK_SIZE);
    for(int i = 0; i < sb->fatblocks; i++) {
	fattable[i].nextblock = -1;
        fattable[i].isFree = 1;
        
       
	}

    directory = malloc(BLOCK_SIZE);
    for(int i=0;i<64;i++)
    {
      strcpy(directory[i].fileName,"");
      directory[i].fileSize = 0;
      directory[i].firstDataBlock = -1;
    }
    

    if(block_write(0, (void*)sb) < 0) {
		
		return -1;
    }

     
 
struct fatrecord *temparray;
struct fatrecord *mem;
int fatsz = sizeof(struct fatrecord);
int perBlockEntry = BLOCK_SIZE/fatsz;
int realBlocksRequired = sb->fatblocks/perBlockEntry;
mem = malloc(sizeof(struct fatrecord)*perBlockEntry);
temparray =  malloc(sizeof(struct fatrecord)*perBlockEntry);
for(int i=0;i<realBlocksRequired;i++)
{
    for(int j=0;j<perBlockEntry;j++)
    {
         temparray[j].nextblock = fattable[(i*perBlockEntry)+j].nextblock;
          temparray[j].isFree = fattable[(i*perBlockEntry)+j].isFree;
    }
    
    memcpy(mem,temparray,perBlockEntry);
    if(block_write(i + 1,(void*)mem) < 0) {

                return -1;
        }
    
    
    
}
    

    if(block_write(FAT_ALLOCATION_BLOCKS+1, (void*)directory) < 0) {
	
	return -1;
   }
    return 0;   

}

int mount_fs(char *disk_name)
{
    if(alreadyOpen == 0)
    {
    if(open_disk(disk_name)<0)
    {
        return -1;
    }
    }
   read_sb = malloc(BLOCK_SIZE);
   if(block_read(0, (void*)read_sb) < 0){
		
		return -1;
	}

  
struct fatrecord *read_mem;
read_fattable = malloc(read_sb->fatblocks*sizeof(struct fatrecord));
int fatsz = sizeof(struct fatrecord);
int perBlockEntry = BLOCK_SIZE/fatsz;
int realBlocksRequired = sb->fatblocks/perBlockEntry;
read_mem = malloc(sizeof(struct fatrecord)*perBlockEntry);

for(int i=0;i<realBlocksRequired;i++)
{
    if(block_read(i + 1, read_mem)<0) {
        
                return -1;
        }
    for(int j=0;j<perBlockEntry;j++)
    {
         struct fatrecord *indexing = malloc(sizeof(struct fatrecord)*j);
         read_fattable[(i*perBlockEntry)+j].nextblock = (&read_mem[j])->nextblock;
         read_fattable[(i*perBlockEntry)+j].isFree = (&read_mem[j])->isFree;

    }

    


}
 
   read_directory = malloc(BLOCK_SIZE);

     if(block_read(FAT_ALLOCATION_BLOCKS+1, (void*)read_directory) < 0) {
        
        return -1;
   }
    
    if(isFileDescriptorExist==0)
    {
    fd = malloc(sizeof(struct filedescriptor) * MAX_OPEN_FILE);
    for(int i=0;i<MAX_OPEN_FILE;i++)
    {
      strcpy(fd[i].fileName,"");

      fd[i].fileDirectoryIndex = -1;
      fd[i].offset = 0;
      fd[i].isActive = 0;
      
    }
    isFileDescriptorExist = 1;
    }
    else
    {
       for(int i=0;i<MAX_OPEN_FILE;i++)
    {
      if(strcmp(fd[i].fileName,"")!=0)
      {
        fd[i].isActive = 1;
      }
     

    }
    }

   

  return 0;

}


int unmount_fs(char *disk_name)
{
     if(block_write(0, (void*)read_sb) < 0){
                
                return -1;
        }


     struct fatrecord *temparray;
struct fatrecord *mem;
int fatsz = sizeof(struct fatrecord);
int perBlockEntry = BLOCK_SIZE/fatsz;
int realBlocksRequired = sb->fatblocks/perBlockEntry;
mem = malloc(sizeof(struct fatrecord)*perBlockEntry);
temparray =  malloc(sizeof(struct fatrecord)*perBlockEntry);
for(int i=0;i<realBlocksRequired;i++)
{
    for(int j=0;j<perBlockEntry;j++)
    {
         temparray[j].nextblock = read_fattable[(i*perBlockEntry)+j].nextblock;
          temparray[j].isFree = read_fattable[(i*perBlockEntry)+j].isFree;
    }

    memcpy(mem,temparray,perBlockEntry);
    if(block_write(i + 1,(void*)mem) < 0) {

                return -1;
        }
    


}

 

     if(block_write(FAT_ALLOCATION_BLOCKS+1, (void*)read_directory) < 0) {
        
        return -1;
   }


   free(read_sb);
   free(read_fattable);
   free(read_directory);
  // free(fd);
        for(int i=0;i<MAX_OPEN_FILE;i++)
    {
      
        fd[i].isActive = 0;
      


    }

  if(close_disk()<0)
  {
     return -1;
  }
  alreadyOpen = 0;

   return 0;

}

int fs_open(char *file_name)
{
   int isFileCreated = 0;
   int directoryInd = -1; 
   
   for(int i = 0; i < TOTAL_FILES; i++) {
         if(strcmp(read_directory[i].fileName,file_name)==0) {

                 isFileCreated=1;
                 directoryInd = i;       
   
                }
        }  

   if(isFileCreated == 1)
   {
      for(int i=0;i<MAX_OPEN_FILE;i++)
      {
        if((strcmp(fd[i].fileName,"")==0) && fd[i].isActive == 0)
        {
        
           //fd[i].fileName = file_name;
           strcpy(fd[i].fileName,file_name);
           fd[i].fileDirectoryIndex = directoryInd;
           fd[i].isActive = 1;
           fd[i].offset = 0;
           return i;
        }

      }

   }
   
   printf("File descriptor can't be created for the file");
    return -1;

}


int fs_close(int fildes)
{
   int descripActive = 1;

   if(fildes>MAX_OPEN_FILE-1)
   {
	return -1;
   }
   
   for(int i=0;i<MAX_OPEN_FILE;i++)
   {
      if(i==fildes && fd[i].isActive==1)
      {
         fd[i].isActive = 0;
         //strcpy(fd[i].fileName)
         strcpy(fd[i].fileName,"");
           fd[i].fileDirectoryIndex = -1;
           //fd[i].isActive = 1;
           fd[i].offset = 0;

         return 0;
      }
    


   }

  return -1;

}

int fs_create(char *file_name)
{
  // int spaceFound = 0;
   if(strlen(file_name)>15)
  {
    printf("invalid file name");
    return -1;
  }
//  if(strcmp(file_name,"") == 0)
//  {
//    printf("match");//
//  }
    for(int i = 0; i < TOTAL_FILES; i++) {
         if(strcmp(read_directory[i].fileName,file_name)==0) {

                       
                    printf("file already exists");

                        return -1;


                }
        }

   	for(int i = 0; i < TOTAL_FILES; i++) {
		if(strcmp(read_directory[i].fileName,"")==0) {	
		
			
                      // spaceFound = 1; 
			strcpy(read_directory[i].fileName, file_name);
			read_directory[i].fileSize     = 0;
			read_directory[i].firstDataBlock = -1;
                        return fs_open(file_name);
			//return 0;
               
             
		}
	}
   

  return -1;

}


int fs_write(int fildes, void *buf, size_t n_byte)
{
     int currentOffset = 0;
     char *inputBuffer = (char*)buf;
     int fileIndex= -1;
     int current_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int new_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int free_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int numberOfCurrentFATBlocks = 0;
     int nextBlock = -1;
     int numberOfFreeFatBlocks = 0;
     int currentFATInd = 0;
     int writtenByte = 0;
     int fileSize = 0;
     size_t offset = 0;
     //printf("|||||||||||||||||||||||||||||    file des%d ||||||||||||||||||||||||||||||||\n",fildes);
     if(fildes>MAX_OPEN_FILE-1)
     {
        return -1;
     }
     for(int i=0;i<MAX_OPEN_FILE;i++)
     {

      if(i==fildes && fd[i].isActive==1)
      {
          currentOffset = fd[i].offset;
          fileIndex= fd[i].fileDirectoryIndex;
          break;
      }
      

     }

     if(fileIndex==-1)
    { 
      return -1;
    }
     
//     printf("offset %d\n",currentOffset);
//     printf("file index %d\n",fileIndex);

    for(int i=0;i<TOTAL_FILES;i++)
    {
      if(i==fileIndex)
      {
        current_FAT_Block_Array[currentFATInd] = read_directory[i].firstDataBlock;
        fileSize = read_directory[i].fileSize;
        break;

      }

    }
//     printf("first fat index %d\n",current_FAT_Block_Array[currentFATInd]);
//    printf("fat table 0 %d\n",read_fattable[0].nextblock);
//    printf("fat table 1 %d\n",read_fattable[1].nextblock);
//   printf("fat table 2 %d\n",read_fattable[2].nextblock);
    for (int i=0;i< read_sb->fatblocks;i++)
    {
         if(read_fattable[i].isFree ==1)
         {
             free_FAT_Block_Array[numberOfFreeFatBlocks++] = i;
         }
     

    }
//     printf("first free fat %d\n",free_FAT_Block_Array[0]);
//     printf("second free fat %d\n",free_FAT_Block_Array[4095]);
    if(current_FAT_Block_Array[0] != -1)
    {   nextBlock = current_FAT_Block_Array[currentFATInd];
       // printf("currrent fat blocks %d\n",number);
	//numberOfCurrentFATBlocks++;
        while(nextBlock != -1)
        { 
         //  printf("in the while \n");
//           printf("next block %d\n",nextBlock);
          numberOfCurrentFATBlocks++;
          nextBlock = read_fattable[nextBlock].nextblock;
          current_FAT_Block_Array[++currentFATInd] = nextBlock;
        }

    }
//    printf("currrent fat blocks %d\n",numberOfCurrentFATBlocks);
//    int current_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
//    int new_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
    int tempBuffSize = 0;
    int numberOfFreeBlocks = 0;
    int remainderBlock = currentOffset % BLOCK_SIZE;
    int remainderSpace = 0;
    int prevLastBlock = numberOfCurrentFATBlocks>0 ? current_FAT_Block_Array[currentFATInd-1]:-1;
    int tempByte = 0;
    if(remainderBlock>0)
    {
      remainderSpace = BLOCK_SIZE - remainderBlock;
      tempBuffSize = remainderSpace < n_byte ? remainderSpace : n_byte;
      char partialBuffer[tempBuffSize];
      memcpy(&partialBuffer,inputBuffer,tempBuffSize);
      char *temppartialbuff = partialBuffer;
      block_write(read_sb->firstdatablockindex+current_FAT_Block_Array[numberOfCurrentFATBlocks-1],temppartialbuff);
      writtenByte += tempBuffSize;
      n_byte = n_byte-tempBuffSize;

    }
// printf("Remainder block %d\n",remainderBlock);
//printf("curr blocks %d\n",numberOfCurrentFATBlocks);
//printf("Remainder space %d\n",remainderSpace);
//printf("previsous block %d\n",prevLastBlock);
    if(n_byte>0)
    {  
       //int castedByte = static_cast<int>(n_byte);
       //float newBlocks = n_byte/100;
       int newBlocks = (n_byte / BLOCK_SIZE) + ((n_byte % BLOCK_SIZE) != 0);

       int managedBlock = numberOfFreeFatBlocks >= newBlocks ? newBlocks : numberOfFreeFatBlocks;
       char *fullBuffer = (malloc(sizeof(char)*4096));
       if(prevLastBlock != -1)
       {
       	  read_fattable[prevLastBlock].nextblock = free_FAT_Block_Array[0];
       }
       else
       {
          read_directory[fileIndex].firstDataBlock = free_FAT_Block_Array[0];
       }
//       printf("prevLastBlock %d\n",prevLastBlock);
  //     printf("nbyte %d\n",n_byte);
    //   printf("managed blocks %d\n",managedBlock);
       for(int i=0;i<managedBlock;i++)
       {  
  //         printf("=========================manage block %d================================\n",free_FAT_Block_Array[i]);
	   tempByte = n_byte > BLOCK_SIZE ? BLOCK_SIZE : n_byte;
           free(fullBuffer);
  //         printf("1");
           *fullBuffer = (malloc(sizeof(char)*tempByte));
    //       printf("2");
           memcpy(fullBuffer,&inputBuffer[tempBuffSize],tempByte);
      //     printf("3");
           int writeStatus = block_write(read_sb->firstdatablockindex+free_FAT_Block_Array[i], fullBuffer);
        //   printf("4");
           //char* read_buffer = malloc(sizeof(char)*tempByte);
          // if(block_read(read_sb->firstdatablockindex+free_FAT_Block_Array[i], read_buffer) < 0){
                //fs_error( "failure to read from block \n");
                //return -1;
          // }
          // else
          // {
            //  printf("Reading buffer %s\n",read_buffer);
          // }
           writtenByte += tempByte;
           tempBuffSize = tempBuffSize+tempByte;
           n_byte = n_byte - tempByte;
           read_fattable[free_FAT_Block_Array[i]].nextblock  = i==managedBlock-1?-1:free_FAT_Block_Array[i+1];
           read_fattable[free_FAT_Block_Array[i]].isFree = 0;
          // printf("Write status %d\n",writeStatus);

       }

    }
    
     
     fd[fildes].offset += writtenByte;
     read_directory[fileIndex].fileSize += writtenByte;
     
    //printf("offset %zu\n",fd[fildes].offset);
   // printf("file size %d\n",read_directory[fileIndex].fileSize);

    
   
    return writtenByte;
}

int fs_read(int fildes, void *buf, size_t n_byte)
{
     int currentOffset = 0;
     char *inputBuffer = (char*)buf;
     int fileIndex= -1;
     int current_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int new_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int free_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int numberOfCurrentFATBlocks = 0;
     int nextBlock = -1;
     int numberOfFreeFatBlocks = 0;
     int currentFATInd = 0;
     int writtenByte = 0;
     int fileSize = 0;
     size_t offset = 0;
     int byteRead = 0;
     int startingBlock = 0;
     //char *readBuffer = (char*)buf;
      if(fildes>MAX_OPEN_FILE-1 || n_byte<=0)
     {
        return -1;
     }
     for(int i=0;i<MAX_OPEN_FILE;i++)
     {

      if(i==fildes && fd[i].isActive==1)
      {
          currentOffset = fd[i].offset;
          fileIndex= fd[i].fileDirectoryIndex;
          break;
      }


     }

     if(fileIndex==-1)
    {
      return -1;
    }
     

    //printf("off %zu\n",currentOffset);
    for(int i=0;i<TOTAL_FILES;i++)
    {
      if(i==fileIndex)
      {
        current_FAT_Block_Array[currentFATInd] = read_directory[i].firstDataBlock;
        fileSize = read_directory[i].fileSize;
        break;

      }

    }
   //printf("sz %d\n",fileSize);   
    byteRead = currentOffset+n_byte > fileSize ? (fileSize-currentOffset +1) : n_byte;
    startingBlock = (currentOffset / BLOCK_SIZE) + ((currentOffset % BLOCK_SIZE) != 0);
  // printf("bt %d\n",byteRead);
  // printf("sb %d\n",startingBlock);
  // printf("cb %d\n",current_FAT_Block_Array[0]);
    if(current_FAT_Block_Array[0] != -1)
    {   nextBlock = current_FAT_Block_Array[currentFATInd];
       
        while(nextBlock != -1)
        {
      
          numberOfCurrentFATBlocks++;
          nextBlock = read_fattable[nextBlock].nextblock;

          current_FAT_Block_Array[++currentFATInd] = nextBlock;
//          printf("block cnt %d\n",numberOfCurrentFATBlocks);
        }

    }
//    printf("bef alloc");
   int requiredBlockRead = numberOfCurrentFATBlocks-startingBlock+1;
   char *blockBuffer = malloc(sizeof(char)*BLOCK_SIZE);
   char *allBlockBuffer = malloc(sizeof(char)*BLOCK_SIZE*requiredBlockRead);
    //printf("after alloc");
    int startIndex = 0;
    int readStartIndex =currentOffset-((startingBlock-1)*BLOCK_SIZE)-1;
    int readEndIndex = readStartIndex+byteRead-1;
    char *checkBuf = "aaaaaa";
   //printf("ind %d\n",readStartIndex);
    for(int i=startingBlock;i<=numberOfCurrentFATBlocks;i++)
    { 
  //     printf("in the loop");
       block_read(read_sb->firstdatablockindex+current_FAT_Block_Array[i-1],blockBuffer);
//       printf("rrb %s\n",blockBuffer);
       memcpy(&allBlockBuffer[startIndex],blockBuffer,BLOCK_SIZE);
      startIndex += BLOCK_SIZE;

    }
//    memcpy(&allBlockBuffer[0],checkBuf,10);
   //char *readBuffer = malloc(sizeof(char)*byteRead);
   memcpy(buf,&allBlockBuffer[readStartIndex],byteRead);


     fd[fildes].offset += byteRead-1;
     
    //printf("Offset------------- %d\n", fd[fildes].offset);
    



    return byteRead;
}


int fs_delete(char *name)
{
     int currentOffset = 0;
     //char *inputBuffer = (char*)buf;
     int fileIndex= -1;
     int current_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int new_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int free_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int numberOfCurrentFATBlocks = 0;
     int nextBlock = -1;
     int numberOfFreeFatBlocks = 0;
     int currentFATInd = 0;
     int writtenByte = 0;
     int fileSize = 0;
     size_t offset = 0;
     int byteRead = 0;
     int startingBlock = 0;
     //char *readBuffer = (char*)buf;
     //int fileIndex = -1;
     for(int i=0;i<TOTAL_FILES;i++)
     {
      if(strcmp(read_directory[i].fileName,name)==0)
      {
        current_FAT_Block_Array[currentFATInd] = read_directory[i].firstDataBlock;
        fileSize = read_directory[i].fileSize;
        fileIndex = i;
        break;

      }

     }
    if(fileIndex == -1)
    {
      return -1;

    }
    for(int i=0;i<MAX_OPEN_FILE;i++)
    {
       if(fd[i].fileDirectoryIndex == fileIndex && fd[i].isActive == 1)
       {
         return -1;

       }
       if(fd[i].fileDirectoryIndex == fileIndex)
       {
           strcpy(fd[i].fileName,"");
           fd[i].fileDirectoryIndex = -1;
           fd[i].offset = 0;
           fd[i].isActive = 0;
       }

    }
    if(current_FAT_Block_Array[0] != -1)
    {   nextBlock = current_FAT_Block_Array[currentFATInd];

        while(nextBlock != -1)
        {

          numberOfCurrentFATBlocks++;
          nextBlock = read_fattable[nextBlock].nextblock;

          current_FAT_Block_Array[++currentFATInd] = nextBlock;
        }

    }

    for(int i=0;i<numberOfCurrentFATBlocks;i++)
    {
      read_fattable[current_FAT_Block_Array[i]].nextblock = -1;
      read_fattable[current_FAT_Block_Array[i]].isFree = 1;

    }
   
    read_directory[fileIndex].fileSize = 0;
    read_directory[fileIndex].firstDataBlock = -1;
    strcpy(read_directory[fileIndex].fileName,"");

   return 0;


}

int fs_get_filesize(int fildes)
{
   if((fildes>(MAX_OPEN_FILE-1)) || (fd[fildes].fileDirectoryIndex==-1) || (fd[fildes].isActive==0))
   {
     return -1;
   }

  return read_directory[fd[fildes].fileDirectoryIndex].fileSize;

}


int fs_lseek(int fildes, off_t offset)
{

   if((fildes>(MAX_OPEN_FILE-1)) || (fd[fildes].fileDirectoryIndex==-1) || (fd[fildes].isActive==0) )
   {
     return -1;
   }
   if(read_directory[fd[fildes].fileDirectoryIndex].fileSize<offset)
   {
     return -1;
   }
   
   fd[fildes].offset = offset;

  return 0;
}

int fs_truncate(int fildes, off_t length)
{
      
     int currentOffset = 0;
     //char *inputBuffer = (char*)buf;
     int fileIndex= -1;
     int current_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int new_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int free_FAT_Block_Array[4096] = { [ 0 ... 4095 ] = -1 };
     int numberOfCurrentFATBlocks = 0;
     int nextBlock = -1;
     int numberOfFreeFatBlocks = 0;
     int currentFATInd = 0;
     int writtenByte = 0;
     int fileSize = 0;
     size_t offset = 0;
     int byteRead = 0;
     int startingBlock = 0;
     off_t truncLength = length;
    if(fildes>MAX_OPEN_FILE-1)
    {
      return -1;
    }

    
      for(int i=0;i<MAX_OPEN_FILE;i++)
     {

      if(i==fildes && fd[i].fileDirectoryIndex != -1 && fd[i].isActive == 1)
      {
          currentOffset = fd[i].offset;
          fileIndex= fd[i].fileDirectoryIndex;
          break;
      }


     }

     if(fileIndex ==-1)
     {
         return -1;
     }

   // printf("off %zu\n",currentOffset);
    for(int i=0;i<TOTAL_FILES;i++)
    {
      if(i==fileIndex)
      {
        current_FAT_Block_Array[currentFATInd] = read_directory[i].firstDataBlock;
        fileSize = read_directory[i].fileSize;
        break;

      }

    }

    if(fileSize<length)
    {
      return -1;
    }
   if(current_FAT_Block_Array[0] != -1)
    {   nextBlock = current_FAT_Block_Array[currentFATInd];

        while(nextBlock != -1)
        {

          numberOfCurrentFATBlocks++;
          nextBlock = read_fattable[nextBlock].nextblock;

          current_FAT_Block_Array[++currentFATInd] = nextBlock;
        }

    }
   int blockIndex = numberOfCurrentFATBlocks-1;
   int tempFileSize = fileSize; 
   while(truncLength>0)
   {
     int byteInBlock = tempFileSize%BLOCK_SIZE == 0?BLOCK_SIZE:tempFileSize%BLOCK_SIZE;
     if(truncLength>byteInBlock)
     {
       read_fattable[current_FAT_Block_Array[blockIndex]].nextblock = -1;
       read_fattable[current_FAT_Block_Array[blockIndex]].isFree = 1;
       blockIndex -= 1;
       tempFileSize -= byteInBlock;
       truncLength -= byteInBlock;

     }
     else
     {
       tempFileSize -= truncLength;
       truncLength = 0;
     }



   }

   read_directory[fileIndex].fileSize = tempFileSize;
   fd[fildes].offset = tempFileSize;


   return 0;

}
