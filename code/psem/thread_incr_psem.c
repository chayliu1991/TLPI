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
#include <pthread.h>


static int glob = 0;
static sem_t sem;


static void * threadFunc(void *arg)
{
    int loops = *((int *) arg);
    int loc, j;

    for (j = 0; j < loops; j++) {
        if (sem_wait(&sem) == -1){ // 阻塞等待信号量大于0之后递减该信号量
			perror("sem_wait");
			exit(EXIT_FAILURE);
		}
            

        loc = glob;
        loc++;
        glob = loc;

        if (sem_post(&sem) == -1){  // 增加信号量
			perror("sem_post");
			exit(EXIT_FAILURE);
		}
            
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t t1, t2;
    int loops, s;

    loops = (argc > 1) ? atoi(argv[1]) : 10000000;

    /* Initialize a semaphore with the value 1 */

    if (sem_init(&sem, 0, 1) == -1){
		perror("sem_init");
		exit(EXIT_FAILURE);
	}
        

    /* Create two threads that increment 'glob' */

    s = pthread_create(&t1, NULL, threadFunc, &loops);
    if (s != 0){
		printf( "%d pthread_create", s);
		exit(EXIT_FAILURE);
	}
        
    s = pthread_create(&t2, NULL, threadFunc, &loops);
    if (s != 0){
		printf( "%d pthread_create", s);
		exit(EXIT_FAILURE);
	}

    /* Wait for threads to terminate */

    s = pthread_join(t1, NULL);
    if (s != 0){
		printf( "%d pthread_join", s);
		exit(EXIT_FAILURE);
	}
    s = pthread_join(t2, NULL);
    if (s != 0){
		printf( "%d pthread_join", s);
		exit(EXIT_FAILURE);
	}

    printf("glob = %d\n", glob);
    exit(EXIT_SUCCESS);
}