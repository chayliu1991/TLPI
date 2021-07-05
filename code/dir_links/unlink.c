#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CMD_SIZE 200
#define BUF_SIZE 1024


void errExit(const char* call)
{
    perror(call);
    exit(1);
}

int main(int argc,char* argv[])
{
    int fd, numBlocks,j;
    char shellCmd[CMD_SIZE];
    char buf[BUF_SIZE];

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s temp-file [num-1kb-blocks] \n",argv[0]);
        numBlocks = (argc > 2) ? atoi(argv[2]) : 100000;
    }

    fd = open(argv[1],O_WRONLY | O_CREAT | O_EXCL,S_IRUSR | S_IWUSR);
    if(fd == -1)
        errExit("open");
    
    if(unlink(argv[1]) == -1)
        errExit("unlink");
    
    for(j = 0;j <numBlocks;j++)
    {
        if(write(fd,buf,BUF_SIZE) != BUF_SIZE)
            errExit("write");
    }

    snprintf(shellCmd,CMD_SIZE,"df -k `dirname %s`",argv[1]);
    system(shellCmd);

    if(close(fd) == -1)
        errExit("close");
    
    printf("************************************ Closeed file descriptor\n");
    system(shellCmd);

    exit(EXIT_SUCCESS);
}
