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
    if (argc != 2 || strcmp(argv[1], "--help") == 0){
		 printf("%s sem-name\n", argv[0]);
		 exit(EXIT_FAILURE);
	}
       

    if (sem_unlink(argv[1]) == -1){
    	 perror("sem_unlink");
    	 exit(EXIT_FAILURE);
    }
        
    exit(EXIT_SUCCESS);
}
