#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    int flags;

    if(argc > 1)
    {
        flags = fcntl(STDOUT_FILENO,F_GETFD);
        if(flags ==-1)
            errExit("fcntl() - F_GETFD");
        
        flags |= FD_CLOEXEC;

        if(fcntl(STDOUT_FILENO,F_SETFD,flags) ==-1)
             errExit("fcntl() - F_SETFD");
    }

    execlp("ls","ls","-l",argv[0],(char*)NULL);

    errExit("execlp");    
}
