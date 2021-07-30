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

static void handler(int sig)
{
    printf("PID %ld: caught signal %2d (%s)\n",(long)getpid(),sig,strsignal(sig));
}

int main(int argc,char* argv[])
{
    pid_t parentPid,childPid;
    int j;
    struct sigaction sa;

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s {d|s... [> sig.log 2&1]}\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    setbuf(stdout,NULL);

    parentPid = getpid();
    printf("PID of parent process is:       %ld\n",(long)parentPid);
    printf("Foreground process group ID is : %ld\n",(long)tcgetpgrp(STDIN_FILENO));

    for(j = 1;j < argc;j++)
    {
        childPid = fork();
        if(childPid == -1)
            errExit("fork()");

        if(childPid == 0)
        {
            if(argv[j][0] == 'd')
            {
                if(setpgid(0,0) == -1)
                    errExit("setpgid()");
            }

            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sa.sa_handler = handler;

            if(sigaction(SIGHUP,&sa,NULL) == -1)
                errExit("sigaction()");
            
            break;
        }
    }

    alarm(60);

    printf("PID=%ld PGID=%ld\n",(long)getpid(),(long)getpgrp());
    for(;;)
    {
        pause();        
    }
    exit(EXIT_SUCCESS);
}