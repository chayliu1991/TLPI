#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

#define SYNC_SIG SIGUSR1
#define BUF_SIZE (1024)


void errExit(const char* call)
{
    perror(call);
    exit(1);
}


char *currTime(const char *format)
{
    static char buf[BUF_SIZE];  /* Nonreentrant */
    time_t t;
    size_t s;
    struct tm *tm;

    t = time(NULL);
    tm = localtime(&t);
    if (tm == NULL)
        return NULL;

    s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);

    return (s == 0) ? NULL : buf;
}

static void handler(int sig)
{
    printf("got signal %d\n",sig);
}

int main(int argc,char* argv)
{
    pid_t pid;
    sigset_t blockMask,oriMask,emptyMask;
    struct sigaction sa;

    setbuf(stdout,NULL);
    
    sigemptyset(&blockMask);
    sigaddset(&blockMask,SYNC_SIG);

    if(sigprocmask(SIG_BLOCK,&blockMask,&oriMask) == -1)
    {
        errExit("sigprocmask()");
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = handler;
    if(sigaction(SYNC_SIG,&sa,NULL) == -1)
    {
        errExit("sigaction()");
    }

    switch(pid = fork())
    {
        case -1:
            errExit("fork");
        case 0:
            printf("[%s %ld] Child Start doing some work\n",currTime("%T"),(long)getpid());
            sleep(2);
            printf("[%s %ld] Child about to signal parent\n",currTime("%T"),(long)getpid());
            if(kill(getppid(),SYNC_SIG) == -1)
            {
                 errExit("kill()");
            }             

            _exit(EXIT_SUCCESS);
        default:
            printf("[%s %ld] Parent about to wait for signal\n",currTime("%T"),(long)getpid());            
            sigemptyset(&emptyMask);
            if(sigsuspend(&emptyMask) == -1 && errno != EINTR)
            {
                errExit("sigsuspend()");
            }
            printf("[%s %ld] Parent got signal\n",currTime("%T"),(long)getpid());

            if(sigprocmask(SIG_SETMASK,&oriMask,NULL) == -1)
            {
                errExit("sigprocmask()");
            }

            exit(EXIT_SUCCESS);
    }    
}