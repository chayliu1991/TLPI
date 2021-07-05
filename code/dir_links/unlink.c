#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



#define CMD_SIZE 200
#define BUF_SIZE 1024

int main(int argc,char* argv[])
{
    int fd, numBlocks;
    char shellCmd[CMD_SIZE];
    char buf[BUF_SIZE];

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s temp-file [num-1kb-blocks] \n",argv[0]);
        numBlocks = (argc > 2) ? atoi(argv[2]) : 100000;
    }

    fd = open();
    exit(EXIT_SUCCESS);
}
