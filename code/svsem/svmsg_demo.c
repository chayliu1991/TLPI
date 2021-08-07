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

#define BUF_SIZE (1024)
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

int main(int argc, char *argv[])
{
    int semid;

    if (argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s init-value\n or: %s semid operation\n", argv[0], argv[0]);
        exit(EXIT_SUCCESS);
    }
        

    if (argc == 2) 
    { 
        union semun arg;

        semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR);
        if (semid == -1)
            errExit("semid");

        arg.val = atoi(argv[1]);
        if (semctl(semid, /* semnum= */ 0, SETVAL, arg) == -1)
            errExit("semctl");

        printf("Semaphore ID = %d\n", semid);

    } else { 

        struct sembuf sop; 

        semid = atoi(argv[1]);

        sop.sem_num = 0;  
        sop.sem_op = atoi(argv[2]);
                                  
        sop.sem_flg = 0;  

        printf("%ld: about to semop at  %s\n", (long) getpid(), currTime("%T"));
        if (semop(semid, &sop, 1) == -1)
            errExit("semop");

        printf("%ld: semop completed at %s\n", (long) getpid(), currTime("%T"));
    }

    exit(EXIT_SUCCESS);
}