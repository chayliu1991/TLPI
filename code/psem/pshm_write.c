#include <semaphore.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    int fd;
    size_t len;                 /* Size of shared memory object */
    char *addr;

    if (argc != 3 || strcmp(argv[1], "--help") == 0)
    {
    	 printf("%s shm-name string\n", argv[0]);
    	 exit(EXIT_FAILURE);
    }
       

    fd = shm_open(argv[1], O_RDWR, 0);      /* Open existing object */
    if (fd == -1)
     {
		 printf("shm_open");
		 exit(EXIT_FAILURE);
	}

    len = strlen(argv[2]);
    if (ftruncate(fd, len) == -1)           /* Resize object to hold string */
     {
		 printf("ftruncate");
		 exit(EXIT_FAILURE);
	}
    printf("Resized to %ld bytes\n", (long) len);
    /*FIXME: above: should use %zu here, and remove (long) cast */

    addr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED){
		 printf("mmap");
		 exit(EXIT_FAILURE);
	}

    if (close(fd) == -1)                    /* 'fd' is no longer needed */
     {
		 printf("close");
		 exit(EXIT_FAILURE);
	}

    printf("copying %ld bytes\n", (long) len);
    /*FIXME: above: should use %zu here, and remove (long) cast */
    memcpy(addr, argv[2], len);             /* Copy string to shared memory */
    exit(EXIT_SUCCESS);
}