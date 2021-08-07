#include <sys/types.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>  
#include <unistd.h>   


#include "semun.h"


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    struct semid_ds ds;
    union semun arg;
    int semid,j;

    if(argc < 3 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s semid\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    semid = atoi(argv[1]);    
    arg.buf = &ds;
    if(semctl(semid,0,IPC_STAT,arg) == -1)
        errExit("semctl()");
    
    if(ds.sem_nsems != argc -2)
        printf("Set contaions %ld semaphores,but %d values were supplied\n",(long)ds.sem_nsems,argc-2);
    
    arg.array = calloc(ds.sem_nsems,sizeof(arg.array[0]));
    if(arg.array == NULL)
        errExit("calloc()");
    
    for(j = 2;j < argc;j++)
        arg.array[j-2] = atoi(argv[j]);
    
    if(semctl(semid,0,SETALL,arg) == -1)
        errExit("semctl()");
    printf("Semaphore values changed (PID=%ld)\n",(long)getpid());

    exit(EXIT_SUCCESS);
}

