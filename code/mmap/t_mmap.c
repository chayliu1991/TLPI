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

#define MEM_SIZE 10

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    char *addr;
    int fd;

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s file [new-value]\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    fd = open(argv[1],O_RDWR);
    if(fd == -1)
        errExit("open()");
    
    addr = mmap(NULL,MEM_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,fd,0);
    if(addr == MAP_FAILED)
        errExit("mmap()");
    
    if(close(fd) == -1)
        errExit("close()");

    printf("Current string = %.*s\n",MEM_SIZE,addr);

    if(argc > 2)
    {
        if(strlen(argv[2]) >= MEM_SIZE)
            printf("'new-value' too large\n");

        memset(addr,0,MEM_SIZE);
        strncpy(addr,argv[2],MEM_SIZE-1);
        if(msync(addr,MEM_SIZE,MS_SYNC) == -1)
            errExit("msync()");

        printf("Copied \"%s\" to shared memory\n",argv[2]);
    }

    exit(EXIT_SUCCESS);
}