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
    sem_t *sem;

    if (argc != 2){
		printf("%s sem-name\n", argv[0]);
		exit(EXIT_FAILURE);
	}
        

    sem = sem_open(argv[1], 0);
    if (sem == SEM_FAILED){
		printf("sem_open");
		exit(EXIT_FAILURE);
	}
        

    if (sem_post(sem) == -1)
    {
		printf("sem_post");
		exit(EXIT_FAILURE);
	}
        
    exit(EXIT_SUCCESS);
}
