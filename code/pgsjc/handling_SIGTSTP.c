#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

static void tstpHandler(int sig)
{
    sigset_t tstpMask,prevMask;
    int saveErrno;
    struct sigaction sa;

    saveErrno = errno;
    printf("Caught SIGTSTP\n");

    if(signal(SIGTSTP,SIG_DFL) == SIG_ERR)
        errExit("signal()");
    
    raise(SIGTSTP);

    sigemptyset(&tstpMask);
    sigaddset(&tstpMask,SIGTSTP);
    if(sigprocmask(SIG_UNBLOCK,&tstpMask,&prevMask) == -1)
        errExit("sigprocmask()");
    
    if(sigprocmask(SIG_SETMASK,&prevMask,NULL) == -1)
        errExit("sigprocmask()");
    
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = tstpHandler;

    if(sigaction(SIGTSTP,&sa,NULL) == -1)
        errExit("sigaction()");
    
    printf("Exiting SIGTSTP handler\n");
    errno = saveErrno;
}

int main(int argc,char* argv[])
{
    struct sigaction sa;

    if(sigaction(SIGTSTP,NULL,&sa) == -1)
        errExit("sigaction()");
    
    if(sa.sa_handler != SIG_IGN)
    {
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = tstpHandler;
        if(sigaction(SIGTSTP,&sa,NULL) == -1)
            errExit("sigaction()");
    }

    for(;;)
    {
        pause();
        printf("Main\n");
    }        
}