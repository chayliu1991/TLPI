#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}


int main(int argc,char* argv[])
{
    int which,prio;
    id_t who;

    if(argc != 4 || strchr("pgu",argv[1][0]) == NULL)
    {
        printf("%s {p|g|u} who priority\n       set priority of : p=process;g=process group;u=processes for user\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    which = (argv[1][0] == 'p') ? PRIO_PROCESS : (argv[1][0] == 'g') ? PRIO_PGRP : PRIO_USER;
    who = atol(argv[2]);
    prio = atoi(argv[3]);

    if(setpriority(which,who,prio) == -1)
        errExit("setpriority()");
    
    errno = 0;
    prio = getpriority(which,who);
    if(prio == -1 && errno != 0)
        errExit("getpriority()");
    
    printf("Nice value = %d\n",prio);
    exit(EXIT_SUCCESS);
}