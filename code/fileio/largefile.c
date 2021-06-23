#define _LARGEFILE64_SOURCE
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


void errExit(const char *caller)
{
    perror(caller);
    exit(1);
}

int main(int argc,char* argv[])
{
    if (argc != 3 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s pathname offset\n", argv[0]);
        exit(0);
    }

    int fd;
    off64_t off;

    fd = open64(argv[1], O_RDWR | O_CREAT, 0644);
    if(fd < 0)
        errExit("open64");

    off = atoll(argv[2]);
    if(lseek64(fd,off,SEEK_SET) < 0)
        errExit("lseek64");
    
    if(write(fd,"test",4) < 0)
        errExit("write");

    exit(EXIT_SUCCESS);
}