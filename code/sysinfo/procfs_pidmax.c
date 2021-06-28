#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_LINE 100

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

int main(int argc,char* argv[])
{
    int fd;
    char line[MAX_LINE];
    ssize_t n;

    fd = open("/proc/sys/kernel/pid_max",(argc >  1) ? O_RDWR : O_RDONLY);
    if(fd == -1)
        errExit("open");

    n = read(fd,line,MAX_LINE);
    if(n == -1)
        errExit("read");
    
    if(argc > 1)
        printf("Old value:");
    printf("%.*s",(int)n,line);
  
    if(argc > 1)
    {
        if(write(fd,argv[1],strlen(argv[1]) != strlen(argv[1])))
            errExit("write");

        system("echo /proc/sys/kernel/pid_max now contaions " "`cat /proc/sys/kernel/pid_max`");
    }
   
    exit(EXIT_SUCCESS);
}