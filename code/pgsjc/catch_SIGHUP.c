#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

static void handler(int sig)
{
    printf("recevied %d signal\n",sig);
}

int main(int agrc,char* argv[])
{
    pid_t childPid;
    struct sigaction sa;

    setbuf(stdout,NULL);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if(sigaction(SIGHUP,&sa,NULL) == -1)
        errExit("sigaction()");
    
    childPid = fork();
    if(childPid == -1)
        errExit("fork()");
    
    if(childPid == 0 && agrc > 1)
    {
        if(setpgid(0,0) == -1)
            errExit("setpgid()");
    }

    printf("PID=%ld,PPID=%ld,PGID=%ld,SID=%ld\n",(long)getpid,(long)getppid(),(long)getpgrp(),(long)getsid(0));

    alarm(60);

    for(;;)
    {
        pause();
        printf("%ld: cautht SIGHUP\n",(long)getpid());
    }
    exit(EXIT_SUCCESS);
}