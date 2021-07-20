#include <unistd.h>
#include <sys/wait.h>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <string.h>

#define CMD_SIZE (200)
#define BUF_SIZE (1024)

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

void printWaitStatus(const char *msg, int status)
{
    if (msg != NULL)
        printf("%s", msg);

    if (WIFEXITED(status)) {
        printf("child exited, status=%d\n", WEXITSTATUS(status));

    } else if (WIFSIGNALED(status)) {
        printf("child killed by signal %d (%s)",
                WTERMSIG(status), strsignal(WTERMSIG(status)));
#ifdef WCOREDUMP        /* Not in SUSv3, may be absent on some systems */
        if (WCOREDUMP(status))
            printf(" (core dumped)");
#endif
        printf("\n");

    } else if (WIFSTOPPED(status)) {
        printf("child stopped by signal %d (%s)\n",
                WSTOPSIG(status), strsignal(WSTOPSIG(status)));

#ifdef WIFCONTINUED     /* SUSv3 has this, but older Linux versions and
                           some other UNIX implementations don't */
    } else if (WIFCONTINUED(status)) {
        printf("child continued\n");
#endif

    } else {            /* Should never happen */
        printf("what happened to this child? (status=%x)\n",
                (unsigned int) status);
    }

    printf("\n");
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

static volatile int numLibeChildren = 0;

static void sigChildHandler(int sig)
{
    int status,savaErrno;
    pid_t childPid;

    savaErrno = errno;

    printf("%s handler : Caught SIGCHLD\n",currTime("%T"));

    while((childPid = waitpid(-1,&status,WNOHANG)) > 0)
    {
        printf("%s handler : Reaped child %ld - ",currTime("%T"),(long)getpid());
        printWaitStatus(NULL,status);
        numLibeChildren --;
    }

    if(childPid == -1 && errno != ECHILD)
        printf("waitpid error\n");
    
    sleep(5);
    printf("%s handler:returning \n",currTime("%T"));
}


int main(int argc,char* argv[])
{
    int j,sigCnt;
    sigset_t blockMask,emptyMask;
    struct sigaction sa;

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {   
        printf("%s child - sleep - time...\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    setbuf(stdout,NULL);
    numLibeChildren = argc - 1;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigChildHandler;
    if(sigaction(SIGCHLD,&sa,NULL) == -1)
    {
        errExit("sigaction()");
    }

    sigemptyset(&blockMask);
    sigaddset(&blockMask,SIGCHLD);
    if(sigprocmask(SIG_SETMASK,&blockMask,NULL) == -1)
    {
        errExit("sigprocmask()");
    }

    for(j = 1;j < argc;j++)
    {
        switch(fork())
        {
            case -1:
                 errExit("fork()");
            case 0:
                sleep(atoi(argv[j]));
                printf("%s Child %d (PID=%ld) exiting\n",currTime("%T"),j,(long)getpid());
                _exit(EXIT_SUCCESS);
            default:
            break;
        }
    }

    sigemptyset(&emptyMask);
    while(numLibeChildren > 0)
    {
        if(sigsuspend(&emptyMask) == -1 && errno != EINTR)
        {
            errExit("sigsuspend()");
        }
        sigCnt++;
    }

    printf("%s All %d children have terminated;SIGCHLD wa caught %d times\n",currTime("%T"),argc-1,sigCnt);
    exit(EXIT_SUCCESS);
}
