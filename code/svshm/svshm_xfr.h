#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>  
#include <unistd.h>  


#define SHM_KEY 0x1234
#define SEM_KEY 0x5678

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)


#define WRITE_SEM 0 
#define READ_SEM 1

#ifndef BUF_SIZE 
#define BUF_SIZE 1024
#endif

struct shmseg 
{ 
    int cnt; 
    char buf[BUF_SIZE]; 
};