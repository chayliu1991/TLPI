#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <errno.h>

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

int main(int argc,char* argv[])
{
    pid_t pid;
    int status = 0;

    if(argc > 1 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s [exit_status]\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    switch(pid = fork())
    {
        case -1:
            errExit("fork()");
        case 0:
            printf("child started with PID:%ld\n",(long)getpid());
            if(argc > 1)
                exit(atoi(argv[1]));
            else
                for(;;)
                    pause();                
            _exit(EXIT_FAILURE);
        default:
            for(;;)
            {
                pid = waitpid(-1,&status,WUNTRACED 
                #ifdef WCONTINUTED
                | WCONTINUED
                #endif
                );

                printf("waitpid() returned :PID:%ld;status=0x%04x (%d%d)\n",(long)pid,(unsigned int)status,status >> 8,status & 0xFF);
                printWaitStatus(NULL,status); 

                if(WIFEXITED(status) || WIFSIGNALED(status))
                    exit(EXIT_SUCCESS);
            }             
    }       
}
  