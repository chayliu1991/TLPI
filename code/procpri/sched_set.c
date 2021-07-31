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

    if(argc < 3 || strchr("rfo",argv[1][0]) == NULL)
    {
        printf("%s policy priority [pid...]\n   policy is 'r'(RR),f(FIFO)"
        #ifdef SCHED_BATCH
        'b'(BATCH)
        #endif
        #ifdef SCHED_IDLE
        'i'(IDLE)
        #endif
            "or 'o'(OTHER)\n",argv[0]);
        
        exit(EXIT_SUCCESS);
    }
    
            
    pol = (argv[1][0] == 'r') ? SCHED_RR : (argv[1][0] == 'f') ? SCHED_FIFO :
    #ifdef SCHED_BATCH
    (argv[1][0] == 'b') ? SCHED_BATCH :
    #endif
    #ifdef SCHED_IDLE
    (argv[1][0] == 'i') ? SCHED_IDLE :
    #endif
        SCHED_OTHER;
    
    sp.sched_priority = atoi(argv[2]);

    for(j = 3; j < argc;j++)
    {
        if(sched_setscheduler(atol(argv[j]),pol,&sp) == -1)
            errExit("sched_setscheduler()");
    }

    exit(EXIT_SUCCESS);
}