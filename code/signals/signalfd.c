#define _GNU_SOURCE

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/signalfd.h>


void errExit(const char* call)
{
    perror(call);
    exit(1);
}

int main(int argc,char* argv[])
{
    sigset_t* mask;
    int sfd,j;
    struct signalfd_siginfo fdsi;
    ssize_t s;

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s : PID = %ld\n",argv[0],(long)(getpid()));
        exit(1);
    }

    sigemptyset(&mask);
    for(j = 1; j < argc;j++)
    {
        sigaddset(&mask,atoi(argv[j]));
    }

    if(sigprocmask(SIG_BLOCK,&mask,NULL) == -1)
    {
        errExit("sigprocmask");
    }


    sfd = signalfd(-1,&mask,0);
    if(sfd == -1)
        errExit("signalfd");

    
    for(;;)
    {
        s = read(sfd,&fdsi,sizeof(struct signalfd_siginfo));
        if(s != sizeof(struct signalfd_siginfo))
            errExit("read");
        
        printf("%s got signal %d",argv[0],fdsi.ssi_signo);
        if(fdsi.ssi_code == SI_QUEUE)
        {
            printf("; ssi_pid = %d;",fdsi.ssi_pid);
            printf("ssi_int = %d;",fdsi.ssi_int);
        }
        printf("\n");
    }

    exit(EXIT_SUCCESS);
}