#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

#define BUF_SIZE (1024)
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


int main(int argc,char*argv[])
{
    int pfd[2];
    int j,dummy;

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s sleep-time\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    setbuf(stdout,NULL);
    
    printf("%s sleep-time...\n",currTime("%T"));

    if(pipe(pfd) == -1)
        errExit("pipe()");
    
    for(j = 1;j < argc;j++)
    {
        switch (fork())
        {
        case -1:
            errExit("fork()");
            break;
        case 0:
            if(close(pfd[0]) == -1)
                errExit("child close()");
            
             /* Child do some work,and lets parent know it's done */

            sleep(atoi(argv[j]));
            printf("%s Child %d (PID=%ld) closing pipe\n",currTime("%T"),j,(long)getpid());

            if(close(pfd[1]) == -1)
                errExit("child close()");
            
            /* Child now carries on to do other things...*/
            _exit(EXIT_SUCCESS);
        default:
            break;
        }
    }  

    if(close(pfd[1]) == -1)
        errExit("parent close()");
    
    /* Parent may do other work,then synchroizes with children*/

    if(read(pfd[0],&dummy,1) != 0)
        errExit("parent read()");
    /* Parent now carries on to do other things...*/

    exit(EXIT_SUCCESS);
}