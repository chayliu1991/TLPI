#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    char *addr;
    int fd;
    struct stat sb;

    if(argc != 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s file\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    fd = open(argv[1],O_RDONLY);
    if(fd == -1)
        errExit("open()");
    
    if(fstat(fd,&sb) == -1)
        errExit("fstat()");

    addr = mmap(NULL,sb.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    if(addr == MAP_FAILED)
        errExit("mmap()");
    
    if(write(STDOUT_FILENO,addr,sb.st_size) != sb.st_size)
        errExit("partial /failed write");

    exit(EXIT_SUCCESS);
}