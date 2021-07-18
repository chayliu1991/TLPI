#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


void errExit(const char* call)
{
    perror(call);
    exit(1);
}

int main(int argc,char* argv)
{
    int istack = 222;
    pid_t pid;

    switch(vfork())
    {
        case -1:
            errExit("vfork()");
        case 0:
            sleep(3);
            write(STDOUT_FILENO,"Child Executing\n",strlen("Child Executing\n"));
            istack *= 4;
            _exit(EXIT_SUCCESS);
        default:
            write(STDOUT_FILENO,"Parent Executing\n",strlen("Parent Executing\n"));
            printf("istack is:%d\n",istack);
            exit(EXIT_SUCCESS);
    }
}