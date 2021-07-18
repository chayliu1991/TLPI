#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv)
{
    printf("Hello World\n");
    write(STDOUT_FILENO,"Ciao\n",strlen("Ciao\n"));

    if(fork() == -1)
    {
        errExit("fork()");
    }


    exit(EXIT_SUCCESS);
}