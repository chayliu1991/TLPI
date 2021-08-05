#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}


#define BUF_SIZE   10


int main(int argc,char*argv[])
{
    int pfd[2];
    char buf[BUF_SIZE];
    ssize_t numRead;

    if(argc !=2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s string\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    if(pipe(pfd) == -1)
        errExit("pipe()");
    
    switch (fork())
    {
    case -1:
        errExit("fork()");
        break;
    case 0:
        if(close(pfd[1]) == -1)
            errExit("child close()");
        for (;;)
        {
            numRead = read(pfd[0],buf,BUF_SIZE);
            if(numRead == -1)
                errExit("child read()");
            if(numRead == 0)
                break;
            if(write(STDOUT_FILENO,buf,numRead) != numRead)
                errExit("child write()");
        }

        write(STDOUT_FILENO,"\n",1);
        if(close(pfd[0]) == -1)
            errExit("child close()");
        _exit(EXIT_SUCCESS);
    default:
        if(close(pfd[0]) == -1)
            errExit("parent close()");
        if(write(pfd[1],argv[1],strlen(argv[1])) != strlen(argv[1]))
            errExit("parent write()");  
        
         if(close(pfd[1]) == -1)
            errExit("parent close()");
         wait(NULL);
         exit(EXIT_SUCCESS);
    }
}