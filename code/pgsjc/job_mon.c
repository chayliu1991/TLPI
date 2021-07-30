#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

static int cmdNum;

static void handler(int sig)
{
    if(getpid() == getpgrp())
        fprintf(stderr,"Terminal FG process group:%ld\n",(long)tcgetpgrp(STDERR_FILENO));
    
    fprintf(stderr,"Process %ld (%d) received signal %d (%s)\n",(long)getpid(),cmdNum,sig,strsignal(sig));

    if(sig == SIGTSTP)
        raise(SIGSTOP);
}

int main(int argc,char* argv[])
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;

    if(sigaction(SIGINT,&sa,NULL) == -1)
        errExit("sigaction()");

    if(sigaction(SIGTSTP,&sa,NULL) == -1)
        errExit("sigaction()");

    if(sigaction(SIGCONT,&sa,NULL) == -1)
        errExit("sigaction()");

    if(isatty(STDIN_FILENO))
    {
        fprintf(stderr,"Terminal FG process group:%ld\n",(long)tcgetpgrp(STDIN_FILENO));
        fprintf(stderr,"Command PID PPID PGRP SID\n");
        cmdNum = 0;
    }
    else
    {
        if(read(STDIN_FILENO,&cmdNum,sizeof(cmdNum)) <= 0)
            errExit("read()");
    }

    cmdNum++;
    fprintf(stderr,"%4d     %5ld %5ld %5ld %5ld\n",cmdNum,(long)getpid(),(long)getppid(),(long)getpgrp(),(long)getsid(0));

    if(!isatty(STDOUT_FILENO))
    {
        if(write(STDOUT_FILENO,&cmdNum,sizeof(cmdNum)) == -1)
            errExit("write()");
    }

    for(;;)
        pause();
}