#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sched.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    int j,pol;
    struct sched_param sp;

    for(j = 1;j < argc;j++)
    {
        pol = sched_getscheduler(atol(argv[j]));
        if(pol == -1)
            errExit("sched_getscheduler()");

        if(sched_getparam(atol(argv[j]),&sp) == -1)
            errExit("sched_getparam()");
        
        printf("%s :%-5s %2d",argv[j],
            (pol == SCHED_OTHER) ? "OTHER" :
            (pol == SCHED_RR) ?     "RR" :
            (pol == SCHED_FIFO) ?  "FIFO" :
            #ifdef SCHED_BATCH
            (pol == SCHED_BATCH) ?  "BATCH" :
            #endif
            #ifdef SCHED_IDLE
            (pol == SCHED_IDLE) ?  "IDLE" :
            #endif     
            "???",sp.sched_priority);
    } 

    exit(EXIT_SUCCESS);
}