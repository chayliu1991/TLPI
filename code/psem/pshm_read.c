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
    char *addr;
    struct stat sb;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
    {
		printf("%s shm-name\n", argv[0]);
		 exit(EXIT_FAILURE);
	}

    fd = shm_open(argv[1], O_RDONLY, 0);    /* Open existing object */
    if (fd == -1)
       {
		 printf("shm_open");
		 exit(EXIT_FAILURE);
	}

    /* Use shared memory object size as length argument for mmap()
       and as number of bytes to write() */

    if (fstat(fd, &sb) == -1)
     {
		 printf("fstat");
		 exit(EXIT_FAILURE);
	}

    addr = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
		 printf("mmap");
		 exit(EXIT_FAILURE);
	}

    if (close(fd) == -1)                    /* 'fd' is no longer needed */
    {
		 printf("close");
		 exit(EXIT_FAILURE);
	}

    write(STDOUT_FILENO, addr, sb.st_size);
    write(STDOUT_FILENO, "\n", 1);
    exit(EXIT_SUCCESS);
}