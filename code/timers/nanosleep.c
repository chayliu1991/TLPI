#define _POSIX_C_SOURCE 199309
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

static void sigintHandler(int sig)
{
    return;
}

int main(int argc,char* argv[])
{
    struct timeval start,finish;
    struct timespec request,remain;
    struct sigaction sa;
    int s;

    if(argc != 3 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s secs nanosecs\n",argv[0]);
        exit(1);
    }

    request.tv_sec = atol(argv[1]);
    request.tv_nsec = atol(argv[2]);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler= sigintHandler;
    if(sigaction(SIGINT,&sa,NULL) == -1)
        errExit("sigaction");

    if(gettimeofday(&start,NULL) == -1)
        errExit("gettimeofday");
    
    for(;;)
    {
        s = nanosleep(&request,&remain);
        if(s ==-1 && errno != EINTR)
        {
            errExit("nanosleep");
        }

        if(gettimeofday(&finish,NULL) == -1)
            errExit("gettimeofday");
        
        printf("sleep for :%9.6f seconds\n",finish.tv_sec-start.tv_sec + (finish.tv_usec-start.tv_usec)/1000000.0);

        if(s == 0)
            break;
        
        printf("Remaining :  %2ld.%09ld\n",(long)remain.tv_sec,remain.tv_nsec);
        request = remain;
    }

    printf("Sleep Completed\n");
    exit(EXIT_SUCCESS);
}