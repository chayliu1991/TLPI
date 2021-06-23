#include <sys/uio.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>


void errExit(const char *caller)
{
    perror(caller);
    exit(1);
}

int main(int argc,char* argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
    {
         printf("%s file\n", argv[0]);
         exit(0);
    }

    int fd;
    struct iovec iov[3];
    struct stat myst;
    int x;

    #define STR_SIZE 100
    char str[STR_SIZE];

    ssize_t numRead, totRequired;

    fd = open(argv[1],O_RDONLY);
    if(fd < 0)
        errExit("open");

    totRequired = 0;

    iov[0].iov_base = &myst;
    iov[0].iov_len = sizeof(struct stat);
    totRequired += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(x);
    totRequired += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = STR_SIZE;
    totRequired += iov[2].iov_len;

    numRead = readv(fd,iov,3);
    if(numRead < 0)
        errExit("readv");
    
    if(numRead < totRequired)
        printf("read fewr bytes than required\n");

    printf("total bytes requested:%ld;bytes read:%ld\n", (long)totRequired, (long)numRead);
    exit(EXIT_SUCCESS);
}
