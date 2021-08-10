#include <semaphore.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>


int main(int argc, char *argv[])
{
    int value;
    sem_t *sem;

    if (argc != 2)
    {
        printf("%s sem-name\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
        

    sem = sem_open(argv[1], 0);
    if (sem == SEM_FAILED){
		printf("sem_open");
		exit(EXIT_FAILURE);
	}

    if (sem_getvalue(sem, &value) == -1){
		printf("sem_getvalue");
		exit(EXIT_FAILURE);
	}

    printf("%d\n", value);
    exit(EXIT_SUCCESS);
}
