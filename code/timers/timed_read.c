#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#define BUF_SIZE 200

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

static void handler(int sig)
{
    printf("Caught signal\n");
}

int main(int argc,char* argv[])
{
    struct sigaction sa;
    char buf[BUF_SIZE];
    ssize_t numRead;
    int savedErrno;

    if(argc > 1 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s [num-secs [restart-flag]] \n",argv[0]);
        exit(1);
    }

    sa.sa_flags = (argc > 2) ? SA_RESTART : 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    if(sigaction(SIGALRM,&sa,NULL) == -1)
        errExit("sigaction");
    
    alarm((argc > 1) ? atoi(argv[1]) : 10);

    numRead = read(STDIN_FILENO,buf,BUF_SIZE-1);
    savedErrno = errno; // 防止 alarm 改变
    alarm(0);
    errno = savedErrno;

    if(numRead == -1)
    {
        if(errno == EINTR)
        {
            printf("Read timed out\n");
        }
        else
        {
             printf("Read error\n");
        }
    }
    else
    {
        printf("Successful read (%ld bytes):%.*s",(long)numRead,(int)numRead,buf);
    }

    exit(EXIT_SUCCESS);
}