#define _POSIX_C_SOURCE  199309L

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define TIMER_SIG SIGRTMAX  
#define BUF_SIZE 1000

int errExit(char* caller)
{
    perror(caller);
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

static void handler(int sig,siginfo_t* si,void* uc)
{
    timer_t *tidptr;
    tidptr = si->si_value.sival_ptr;

    printf("[%s] Got signal %d \n",currTime("%T"),sig);
    printf("    *sigval_ptr    = %ld\n",(long)*tidptr);
    printf("        timer_getoverrun() = %d \n",timer_getoverrun(*tidptr));
}


void itimerspecFromStr(char *str, struct itimerspec *tsp)
{
    char *dupstr ,*cptr, *sptr;

    dupstr = strdup(str);

    cptr = strchr(dupstr, ':');
    if (cptr != NULL)
        *cptr = '\0';

    sptr = strchr(dupstr, '/');
    if (sptr != NULL)
        *sptr = '\0';

    tsp->it_value.tv_sec = atoi(dupstr);
    tsp->it_value.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;

    if (cptr == NULL) {
        tsp->it_interval.tv_sec = 0;
        tsp->it_interval.tv_nsec = 0;
    } else {
        sptr = strchr(cptr + 1, '/');
        if (sptr != NULL)
            *sptr = '\0';
        tsp->it_interval.tv_sec = atoi(cptr + 1);
        tsp->it_interval.tv_nsec = (sptr != NULL) ? atoi(sptr + 1) : 0;
    }
    free(dupstr);
}


int main(int argc,char* argv[])
{
    struct itimerspec ts;
    struct sigaction sa;
    struct sigevent sev;
    timer_t *tidlist;
    int j;

    if(argc < 2)
    {
        printf("%s secs [/nsecs][:int-secs[/int-nsecs]]...\n",argv[0]);
        exit(1);
    }

    tidlist = calloc(argc - 1, sizeof(timer_t));
    if(tidlist == NULL)
        errExit("calloc");

    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigemptyset(&sa.sa_mask);

    if(sigaction(TIMER_SIG,&sa,NULL))
    {
        errExit("sigaction");
    }

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = TIMER_SIG;

    for (j = 0; j < argc - 1;j++)
    {
        itimerspecFromStr(argv[j+1],&ts);
        sev.sigev_value.sival_ptr = &tidlist[j];


        if(timer_create(CLOCK_REALTIME,&sev,&tidlist[j]) == -1)
            errExit("timer_create");

        printf("Timer ID %ld,(%s)\n",(long)tidlist[j],argv[j+1]);

        if(timer_settime(tidlist[j],0,&ts,NULL) == -1)
             errExit("timer_settime");
    }

    for (;;)
        pause();
}