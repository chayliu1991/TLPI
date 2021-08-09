#include <mqueue.h>
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

int main(int argc,char* argv[])
{
    if(argc != 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s ms-name\n",argv[0]);
        exit(EXIT_SUCCESS);        
    }

    if(mq_unlink(argv[1]) == -1)
        errExit("mq_unlink()");

    exit(EXIT_SUCCESS);
}