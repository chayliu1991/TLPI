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
    union semun arg,dummy;
    int semid,j;

    if(argc != 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s semid\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    semid = atoi(argv[1]);
    
    arg.buf = &ds;
    if(semctl(semid,0,IPC_STAT,arg) == -1)
        errExit("semctl()");
    
    printf("Semaphore changed : %s",ctime(&ds.sem_ctime));
    printf("Last semop : %s",ctime(&ds.sem_otime));

    arg.array = calloc(ds.sem_nsems,sizeof(arg.array[0]));
    if(arg.array == NULL)
        errExit("calloc()");

    if(semctl(semid,0,GETALL,arg) == -1)
        errExit("semctl() GETALL");
    
    printf("Sem # Value SEMPID SEMNCNT SEMZCNT\n");

    for(j = 0;j < ds.sem_nsems;j++)
    {
        printf("%3d %5d %5d %5d %5d\n",j,arg.array[j],
        semctl(semid,j,GETPID,dummy),
        semctl(semid,j,GETNCNT,dummy),
        semctl(semid,j,GETZCNT,dummy));
    }

    exit(EXIT_SUCCESS);
}

