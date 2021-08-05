#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char*argv[])
{
    int pfd[2];
    
    if(pipe(pfd) == -1)
        errExit("pipe()");

    switch(fork())
    {
        case -1:
            errExit("fork()");
        case 0:
            if(close(pfd[0]) == -1)
                errExit("close() 1");
            
            /* Duplicate stdout on write end of pipe;close duplicated descriptor */

            if(pfd[1] != STDOUT_FILENO)
            {
                if(dup2(pfd[1],STDOUT_FILENO) == -1)
                    errExit("dup2() 1");
                if(close(pfd[1]) == -1)
                    errExit("close() 2");
            }

            execlp("ls","ls",(char*)NULL);
            errExit("execlp ls");
        default:
        break;
    }  

    switch(fork()) 
    {
        case -1:
            errExit("fork()");
        case 0:
            if(close(pfd[1]) == -1)
                errExit("close() 3");
            
            /* Duplicate stdin on read end of pipe;close duplicated descriptor */

            if(pfd[0] != STDOUT_FILENO)
            {
                if(dup2(pfd[0],STDIN_FILENO) == -1)
                    errExit("dup2() 2");
                if(close(pfd[0]) == -1)
                    errExit("close() 4");
            }
            execlp("wc","wc","-l",(char*)NULL);
            errExit("execlp wc");
        default:
            break;
    } 

    if(close(pfd[0]) == -1)
        errExit("close() 5");
    if(close(pfd[1]) == -1)
        errExit("close() 6");
    if(wait(NULL) == -1)
        errExit("wait() 1");
    if(wait(NULL) == -1)
        errExit("wait() 2");
    
    exit(EXIT_SUCCESS);
}