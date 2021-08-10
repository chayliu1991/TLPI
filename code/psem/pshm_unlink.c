#include <semaphore.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}


int main(int argc,char* argv[])
{
    if(argc != 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s shm-name \n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    if(shm_unlink(argv[1]) == -1)
        errExit("shm_unlink()");

    exit(EXIT_SUCCESS);
}