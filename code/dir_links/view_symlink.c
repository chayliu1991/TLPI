#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ftw.h>
#include <errno.h>
#include <limits.h>

#define BUF_SIZE PATH_MAX

void errExit(const char* call)
{
    perror(call);
    exit(1);
}

void fatal(const char* err)
{
    printf("error:%s\n",err);
    exit(1);
}

int main(int argc,char* argv[])
{
    struct stat statbuf;
    char buf[BUF_SIZE];
    ssize_t numBytes;

    if(argc != 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s pathname \n",argv[0]);
        exit(1);
    }

    if(lstat(argv[1],&statbuf) < 0)
        errExit("lstat");

    if(!S_ISLNK(statbuf.st_mode))
    {
        char err[BUF_SIZE] = {0};
        snprintf(err, BUF_SIZE, "%s is not a symbolic link", argv[1]);
        fatal(err);
    }

    numBytes = readlink(argv[1],buf,BUF_SIZE-1);
    if(numBytes < 0)
        errExit("readlink");
    buf[numBytes] = '\0';

    printf("readlink:%s  -->  %s\n",argv[1],buf);
    if(realpath(argv[1],buf) == NULL)
        errExit("realpath");
    printf("realpath:%s --> %s\n",argv[1],buf);

    exit(EXIT_SUCCESS);
}