#include <stdio.h>
#include "fs.h"
#include <string.h>
#include <stdlib.h>
int main()
{

int result = make_fs("simfile");
printf("\n");
if(result == 0)
{
 printf("File system named simfile is created successfully\n");

}
else
{
 printf("File system named simfile creation is failed\n");
 return -1;
}

printf("\n");

int result2 = mount_fs("simfile");

if(result2 == 0)
{
 printf("File system named simfile is mounted successfully\n");

}
else
{
 printf("File system named simfile is not mounted successfully\n");
 return -1;
}

printf("\n");

int result4 = fs_create("first_file");
int result5 = fs_create("second_file");
int result6 = fs_create("third_file");
if(result4>-1)
{
 printf("A file named first_file is created successfully. The file descriptor is %d\n",result4);

}
 else
{
 printf("A file named first_file creation is failed\n");
 return -1;
}
printf("\n");
if(result5>-1)
{
 printf("A file named second_file is created successfully. The file descriptor is %d\n",result5);

}
 else
{
 printf("A file named second_file creation is failed\n");
 return -1;
}
printf("\n");
if(result6>-1)
{
 printf("A file named third_file is created successfully. The file descriptor is %d\n",result6);

}
 else
{
 printf("A file named third_file creation is failed\n");
 return -1;
}
printf("\n");

size_t readBufferSize =20000;
char *inputBuffer = "aaaaddddddddddddddddddxxccccccccvvvvvvvvvvvvvvvvvvvvvvvvbbbbbbbbbbbbbbbbbbbqqqqqqqqqqmnmnmnmnmnmnmnmcvbdddddddddddddddddddvvvvvvvvvvvvvvvvvvvvvvvvxxxxxxxxxxaaaa";
size_t n_byte = sizeof(char)*strlen(inputBuffer);
void *readBuffer = malloc(sizeof(char)*readBufferSize);
int byteWrite = 0;
for(int i =0;i<44;i++)
{
byteWrite += fs_write(result5,inputBuffer,n_byte);
}

printf("Total byte written for the file descriptor %d is %d\n", result5,byteWrite);
printf("\n");
byteWrite = 0;
for(int i =0;i<60;i++)
{
byteWrite +=fs_write(result4,inputBuffer,n_byte);
}
printf("Total byte written for the file descriptor %d is %d\n", result4,byteWrite);
printf("\n");

int truncResult = fs_truncate(result5,5000);
if(truncResult==0)
{
printf("The truncate operation for the file descriptor %d is successful\n", result5);
}
else
{
printf("The truncate operation for the file descriptor %d is failed\n", result5);
return -1;
}
printf("\n");
byteWrite = 0;
for(int i =0;i<60;i++)
{
byteWrite += fs_write(result6,inputBuffer,n_byte);
}
printf("Total byte written for the file descriptor %d is %d\n", result6,byteWrite);
printf("\n");
int flSize = fs_get_filesize(result6);
if(flSize>-1)
{
printf("File Size for the file descriptor %d is %d\n",result6,flSize);
}
else
{
printf("Invalid file descriptor\n");
return -1;
}
printf("\n");
int seekResult = fs_lseek(result6,8500);
if(seekResult == 0)
{
printf("The seek operation for the file descriptor %d is successful\n", result6);
}
else
{
printf("The seek operation for the file descriptor %d is failed\n", result6);
return -1;
}
printf("\n");
printf("Total byte read for the file descriptor %d is %d\n",result6,fs_read(result6,readBuffer,readBufferSize));
printf("\n");
int unmountResult = unmount_fs("simfile");

if(unmountResult == 0)
{
printf("The file system named simfile is unmounted successfully\n");
}
else
{
printf("The unmount operation for the file system named simfile is failed\n");
return -1;
}
printf("\n");
int result7 = mount_fs("simfile");
if(result7 == 0)
{
 printf("File system named simfile is mounted successfully\n");

}
else
{
 printf("Mounting is failed for the file system named simfile\n");
 return -1;
}
printf("\n");
int result8 = fs_create("fourth_file");
if(result8>-1)
{
 printf("A file named fourth_file is created successfully. The file descriptor is %d\n",result8);

}
else
{
 printf("A file named fourth_file creation is failed.\n");
 return -1;
}
printf("\n");
byteWrite = 0;
for(int i =0;i<60;i++)
{
byteWrite += fs_write(result8,inputBuffer,n_byte);
}
printf("Total byte written for the file descriptor %d is %d\n", result8,byteWrite);
printf("\n");
int closeresult = fs_close(result4);
if(closeresult ==0)
{
printf("The close operation for the file descriptor %d is successful\n", result4);
}
else
{
printf("The close operation for the file descriptor %d is failed\n", result4);
return -1;
}
printf("\n");
int deleteresult = fs_delete("first_file");

if(deleteresult ==0)
{
printf("The delete operation for the file named first_file is successful\n");
}
else
{
printf("The delete operation for the file named first_file is failed\n");
return -1;
}
printf("\n");
int result9 = fs_create("fifth_file");
if(result9>-1)
{
 printf("A file named fifth_file is created successfully. The file descriptor is %d\n",result9);

}
else
{
 printf("A file named fifth_file creation is failed.\n");
 return -1;
}
printf("\n");
byteWrite = 0;
for(int i =0;i<60;i++)
{
byteWrite += fs_write(result9,inputBuffer,n_byte);
}
printf("Total byte written for the file descriptor %d is %d\n", result9,byteWrite);
printf("\n");
return 0;
}
