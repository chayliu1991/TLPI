#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int agrc,char* argv[])
{
    if(fork() != 0)
        _exit(EXIT_SUCCESS);
    
    if(setsid() == -1)
        errExit("setsid()");
    
    printf("PID = %ld,PGID=%ld,SID=%ld\n",(long)getpid(),(long)getpgrp(),(long)getsid(0));

    if(open("/dev/tty",O_RDWR) == -1)
        errExit("open() /dev/tty");
    
    exit(EXIT_SUCCESS);
}