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
#include <pthread.h>

#define BUF_SIZE 1000

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int expireCnt = 0;

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


static void threadFunc(union sigval sv)
{ 
    timer_t *tidptr;
    int s;

    tidptr = sv.sival_ptr;

    printf("[%s] Thread notify\n", currTime("%T"));
    printf("    timer ID=%ld\n", (long) *tidptr);
    printf("    timer_getoverrun()=%d\n", timer_getoverrun(*tidptr));

    s = pthread_mutex_lock(&mtx);
    if (s != 0)
        errExit("pthread_mutex_lock");

    expireCnt += 1 + timer_getoverrun(*tidptr);

    s = pthread_mutex_unlock(&mtx);
    if (s != 0)
        errExit("pthread_mutex_unlock");

    s = pthread_cond_signal(&cond);
    if (s != 0)
        errExit("pthread_cond_signal");
}

int main(int argc, char *argv[])
{
    struct sigevent sev;
    struct itimerspec ts;
    timer_t *tidlist;
    int s, j;

    if (argc < 2)
    {
        printf("%s secs[/nsecs][:int-secs[/int-nsecs]]...\n", argv[0]);
        exit(1);
    }			
	 
    tidlist = calloc(argc - 1, sizeof(timer_t));
    if (tidlist == NULL)
        errExit("malloc");

    sev.sigev_notify = SIGEV_THREAD;            
    sev.sigev_notify_function = threadFunc;     
    sev.sigev_notify_attributes = NULL;

    for (j = 0; j < argc - 1; j++) {
	 
        itimerspecFromStr(argv[j + 1], &ts);

        sev.sigev_value.sival_ptr = &tidlist[j];
        if (timer_create(CLOCK_REALTIME, &sev, &tidlist[j]) == -1)		 
            errExit("timer_create");
        printf("Timer ID: %ld (%s)\n", (long) tidlist[j], argv[j + 1]);

																   
        if (timer_settime(tidlist[j], 0, &ts, NULL) == -1)		 
            errExit("timer_settime");
		 
    }

    s = pthread_mutex_lock(&mtx);
    if (s != 0)	 
        errExit("pthread_mutex_lock");
	 

    for (;;) {	 
        s = pthread_cond_wait(&cond, &mtx);
        if (s != 0)		 
            errExit("pthread_cond_wait");
		 
        printf("main(): expireCnt = %d\n", expireCnt);
    }
}