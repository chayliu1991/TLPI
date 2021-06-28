#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

static void fpathconfPrint(const char* msg,int fd,int name)
{
    long lim;
    errno = 0;
    lim = fpathconf(fd, name);
    if(lim != -1)
    {
        printf("%s %ld\n",msg,lim);
    }
    else
    {
        if(errno == 0)
            printf("%s (indeterminate)\n",msg);
        else
            errExit("fpathconf %s");
    }
}

int main(int argc,char* argv[])
{
    fpathconfPrint("_PC_NAME_MAX",STDIN_FILENO,_PC_NAME_MAX);
    fpathconfPrint("_PC_PATH_MAX",STDIN_FILENO,_PC_PATH_MAX);
    fpathconfPrint("_PC_PIPE_BUF",STDIN_FILENO,_PC_PIPE_BUF);

    exit(EXIT_SUCCESS);
}

