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

int main(int argc,char* argv[])
{
    int numDead;
    pid_t childPid;
    int j;

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s sleep-time...\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    setbuf(stdout,NULL);
    
    for(j = 1;j < argc;++j)
    {
        switch(fork())
        {
            case -1:
                errExit("fork()");
            case 0:
                printf("[%s] child %d started with PID %ld,sleeping %s seconds\n",currTime("%T"),j,(long)getpid(),argv[j]);
                sleep(atoi(argv[j]));
                _exit(EXIT_SUCCESS);
            default:
                break;
        }
    }
    
    numDead = 0;
    for(;;)
    {
        childPid = wait(NULL);
        if(childPid == -1)
        {
            if(errno == ECHILD)
            {
                printf("No more children - bye\n");
                exit(EXIT_SUCCESS);
            }
            else
            {
                errExit("wait()");
            }   
        }

        numDead ++;
        printf("[%s] wait returned child PID %ld(numDead=%d)\n",
        currTime("%T"),(long)childPid,numDead);
}

    exit(EXIT_SUCCESS);    
}
