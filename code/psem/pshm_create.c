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


static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-cx] shm-name size [octal-perms]\n", progName);
    fprintf(stderr, "    -c   Create shared memory (O_CREAT)\n");
    fprintf(stderr, "    -x   Create exclusively (O_EXCL)\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int flags, opt, fd;
    mode_t perms;
    size_t size;
    void *addr;

    flags = O_RDWR;
    while ((opt = getopt(argc, argv, "cx")) != -1) 
    {
        switch (opt) 
        {
        case 'c':   
            flags |= O_CREAT;           
            break;
        case 'x':   
            flags |= O_EXCL;            
            break;
        default:    
            usageError(argv[0]);
        }
    }

    if (optind + 1 >= argc)
    {
		 printf("%s",argv[0]);
		 exit(EXIT_FAILURE);
	}
       

    size = atoi(argv[optind + 1]);
    perms = (argc <= optind + 2) ? (S_IRUSR | S_IWUSR) : atoi(argv[optind + 2]);

    /* Create shared memory object and set its size */

    fd = shm_open(argv[optind], flags, perms);
    if (fd == -1){
		 printf("shm_open");
		 exit(EXIT_FAILURE);
	}
       

    if (ftruncate(fd, size) == -1){
		 printf("ftruncate");
		 exit(EXIT_FAILURE);
	}
       

    /* Map shared memory object */

    addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED)
    {
		 printf("mmap");
		 exit(EXIT_FAILURE);
	}
       

    exit(EXIT_SUCCESS);
}