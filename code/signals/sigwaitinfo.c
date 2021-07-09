#define _GNU_SOURCE

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>

void errExit(const char* call)
{
    perror(call);
    exit(1);
}


int main(int argc,char* argv[])
{
    int sig;
    siginfo_t si;
    sigset_t allSigs;

    if(argc > 1 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s [decay-secs]\n",argv[0]);
        exit(1);
    }

    printf("%s : PID is :%ld \n",argv[0],(long)getpid());

    //@ 阻塞所有信号，除了 SIGKILL 和 SIGSTOP

    sigfillset(&allSigs);
    if(sigprocmask(SIG_SETMASK,&allSigs,NULL) == -1)
        errExit("sigprocmask");

    printf("%s signals blocked\n",argv[0]);

    if(argc > 1)
    {
        printf("%s : about to decay %s seconds\n",argv[0],argv[1]);
        sleep(atoi(argv[1]));
        printf("%s:finished delay\n",argv[0]);
    }

    for (;;)
    {
        sig = sigwaitinfo(&allSigs,&si);
        if(sig == -1)
            errExit("sigwaitinfo");
        
        if(sig == SIGINT || sig == SIGTERM)
            exit(EXIT_SUCCESS);
    }

    printf("got signal:%d(%s)\n",sig,strsignal(sig));
    printf("        si_signo=%d,si_code%d(%s),si_value=%d\n",si.si_signo,si.si_code,(si.si_code == SI_USER) ? "SI_USER":
            (si.si_code == SI_QUEUE) ? "SI_QUEUE" : "other",si.si_value.sival_int);

    printf("    si_pid%ld,si_uid=%ld\n",(long)si.si_pid,(long)si.si_uid);
    exit(EXIT_SUCCESS);
}