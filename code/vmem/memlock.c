#define _BSD_SOURCE
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
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

static void displayMincore(char* addr,size_t length)
{
    unsigned char *vec;
    long pageSize, numPages,j;

    pageSize = sysconf(_SC_PAGESIZE);

    numPages = (length + pageSize - 1) / pageSize;
    vec = malloc(numPages);
    if(vec == NULL)
        errExit("malloc()");
    
    if(mincore(addr,length,vec) == -1)
        errExit("mincore()");

    for (j = 0; j < numPages;j++)
    {
        if(j % 64 == 0)
        {
            printf("%s%10p:",(j == 0) ? "" : "\n",addr+(j*pageSize));
        }

        printf("%c",(vec[j] & 1) ? '*' : '.');
    }
    printf("\n");
    free(vec);
}

int main(int argc, char *argv[])
{
    char *addr;
    size_t len, lockLen;
    long pageSize, stepSize, j;

    if(argc != 4 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s num-pages lock-page-step lock-page-len",argv[0]);
        exit(EXIT_SUCCESS);
    }

    pageSize = sysconf(_SC_PAGESIZE);
    if(pageSize == -1)
        errExit("sysconf()");

    len = atoi(argv[1]) * pageSize;
    stepSize = atoi(argv[2]) * pageSize;
    lockLen = atoi(argv[3]) * pageSize;

    addr = mmap(NULL,len,PROT_READ,MAP_SHARED | MAP_ANONYMOUS,-1,0);
    if(addr == MAP_FAILED)
        errExit("mmap()");

    printf("Allocated %ld(%#lx) bytes starting at %p\n", (long)len,(unsigned long)len,addr);

    printf("Before lock:\n");
    displayMincore(addr,len);

    for (j = 0; j + lockLen <= len;j += stepSize)
    {
        if(mlock(addr+j,lockLen) == -1)
            errExit("mlock()");
    }

        printf("After lock:\n");
    displayMincore(addr,len);

    exit(EXIT_SUCCESS);
}