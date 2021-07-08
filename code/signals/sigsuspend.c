#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

static volatile sig_atomic_t gotSigquit = 0;

void errExit(const char* call)
{
    perror(call);
    exit(1);
}


static void handler(int sig)
{
    printf("Caught signal %d (%s) \n",sig,strsignal(sig));

    if(sig == SIGQUIT)
        gotSigquit = 1;
}

int main(int argc,char* argv[])
{
    int loopNum;
    time_t startTime;
    sigset_t oriMask, blockMask;
    struct sigaction sa;

    sigemptyset(&blockMask);
    sigaddset(&blockMask,SIGINT);
    sigaddset(&blockMask,SIGQUIT);

    if (sigprocmask(SIG_BLOCK,&blockMask,&oriMask) == -1)
        errExit("sigprocmask");
    
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    if (sigaction(SIGINT,&sa,NULL) == -1)
        errExit("sigaction");
    
    if (sigaction(SIGQUIT,&sa,NULL) == -1)
        errExit("sigaction");

    for (loopNum = 1; !gotSigquit;loopNum++)
    {
        printf("=== LOOP %d\n",loopNum);

        for (startTime = time(NULL); time(NULL) < startTime + 4;)
            continue;
        

        if(sigsuspend(&oriMask) == -1 && errno != EINTR)
            errExit("sigsuspend");
    }
    
    if (sigprocmask(SIG_SETMASK,&oriMask,NULL) == -1)
        errExit("sigprocmask");


    exit(EXIT_SUCCESS);
}