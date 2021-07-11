#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum
{
    False = 0,
    True
}Boolean;


int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

static volatile sig_atomic_t gotAlarm = 0;


static void displayTimes(const char* msg,Boolean includeTimer)
{
    struct itimerval itv;
    static struct timeval start;
    struct timeval curr;
    static int callNum = 0;


    if(callNum == 0)
    {
        if(gettimeofday(&start,NULL) == -1)
        {
            errExit("gettimeofday");
        }
    }

    if(callNum % 20 == 0)
    {
        printf("    Elapsed Value Interval\n");
    }

    if(gettimeofday(&curr,NULL) == -1)
    {
        errExit("gettimeofday");
    }

    printf("% -7s %6.2f",msg,curr.tv_sec - start.tv_sec + (curr.tv_sec - start.tv_sec)/1000000.0);

    if(includeTimer)
    {
        if(getitimer(ITIMER_REAL,&itv) == -1)
        {
            errExit("getitimer");
        }

        printf("    %6.2f   %6.2f",itv.it_value.tv_sec + itv.it_value.tv_usec / 1000000.0,
                            itv.it_interval.tv_sec + itv.it_interval.tv_usec / 1000000.0);
    }

    printf("\n");
    callNum++;
}

static void sigalrmHandler(int sig)
{
    gotAlarm = 1;
}

int main(int argc,char* argv[])
{
    struct itimerval itv;
    clock_t prevClock;
    int maxSigs;
    int sigCnt;
    struct sigaction sa;

    if(argc > 1 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s  [secs [int-secs [int-usecs]]]\n",argv[0]);
        exit(1);
    }

    sigCnt = 0;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigalrmHandler;

    if(sigaction(SIGALRM,&sa,NULL) == -1)
        errExit("sigaction");

    maxSigs = (itv.it_interval.tv_sec == 0 && itv.it_interval.tv_usec == 0) ? 1 : 3;
    displayTimes("START: ",False);

    itv.it_value.tv_sec = (argc > 1) ? atol(argv[1]) : 2;
    itv.it_value.tv_usec = (argc > 2) ? atol(argv[2]) : 0;
    itv.it_interval.tv_sec = (argc > 3) ? atol(argv[1]) : 0;
    itv.it_interval.tv_usec = (argc > 4) ? atol(argv[2]) : 0;

    if(setitimer(ITIMER_REAL,&itv,0) == -1)
        errExit("setitimer");

    prevClock = clock();
    sigCnt = 0;

    for(;;)
    {        
        while(((clock() - prevClock) * 10 / CLOCKS_PER_SEC) < 5)
        {
            if(gotAlarm)
            {
                gotAlarm = 0;
                displayTimes("ARARM: ",True);

                sigCnt ++;
                if(sigCnt >= maxSigs)
                {
                    printf("that's all folks\n");
                    exit(EXIT_SUCCESS);
                }
            }
        }

        prevClock = clock();
        displayTimes("Main: ",True);
    }
}