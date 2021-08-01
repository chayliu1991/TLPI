#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BD_NO_CHDIR           01
#define BD_NO_CLOSE_FILES     02
#define BD_NO_REOPEN_STD_FDS  04
#define BD_NO_UMASK0         010
#define BD_MAX_CLOSE        8192


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int becomeDaemon(int flags)
{
    int maxfd,fd;

    switch(flags)
    {
    case -1: 
        return -1;
    case 0:
        break;
    default:
        _exit(EXIT_SUCCESS);
    }

    if(setsid() == -1)
    {
        return -1;
    }

    switch(fork())
    {
    case -1: 
        return -1;
    case 0:
        break;
    default:
        _exit(EXIT_SUCCESS);
    }

    if(!(flags & BD_NO_UMASK0))
        umask(0);
    
    if(!(flags & BD_NO_CHDIR))
        chdir("/");
    
    if(!(flags & BD_NO_CLOSE_FILES))
    {
        maxfd = sysconf(_SC_OPEN_MAX);
        if(maxfd == -1)
            maxfd = BD_MAX_CLOSE;
        
        for(fd = 0;fd < maxfd;fd++)
            close(fd);
    }

    if(!(flags & BD_NO_REOPEN_STD_FDS))
    {
        close(STDIN_FILENO);

        fd = open("/dev/null",O_RDWR);

        if(fd != STDIN_FILENO)
            return -1;
        
        if(dup2(STDIN_FILENO,STDOUT_FILENO) != STDOUT_FILENO)
            return -1;
        
        if(dup2(STDIN_FILENO,STDERR_FILENO) != STDERR_FILENO)
            return -1;
    }

    return 0;
}

int main(int argc,char* argv[])
{
    becomeDaemon(0);

    sleep((argc > 1) ? atoi(argv[1]) : 20);

    exit(EXIT_SUCCESS);
}