#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <errno.h>


#ifndef CHILD_SIG
#define CHILD_SIG SIGUSR1
#endif

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

static int childfunc(void* arg)
{    
    if(close(*((int*)arg)) == -1)
        errExit("close()");
    return 0;
}



int main(int argc,char* argv[])
{
    const int STACK_SIZE = 65536;
    char* stack;
    char* stacktop;
    int s,fd,flags;

    fd = open("/dev/null",O_RDWR);
    if(fd == -1)
        errExit("open()");
    
    flags = (argc > 1) ? CLONE_FILES : 0;

    stack = malloc(STACK_SIZE);
    if(stack == NULL)
        errExit("malloc()");
    
    stacktop = stack + STACK_SIZE;

    if(CHILD_SIG != 0 && CHILD_SIG != SIGCHLD)
    {
        if(signal(CHILD_SIG,SIG_IGN) == SIG_ERR)
            errExit("signal()");
    }

    if(clone(childfunc,stacktop,flags | CHILD_SIG,(void*)&fd) == -1)
        errExit("clone()");
    
    if(waitpid(-1,NULL,(CHILD_SIG != SIGCHLD) ? __WCLONE : 0) == -1)
        errExit("waitpid()");
    
    printf("child has terminated\n");

    s = write(fd,"x",1);
    if(s == -1 && errno == EBADF)
    {
        printf("file descirption %d has been closed\n",fd);
    }
    else if(s == -1)
    {
        printf("write() on file description %d failed,unexceptedlly (%s)\n",fd,strerror(errno));
    }
    else
    {
        printf("write() on file descriptor %d succeed\n",fd);
    }

    exit(EXIT_SUCCESS);
}