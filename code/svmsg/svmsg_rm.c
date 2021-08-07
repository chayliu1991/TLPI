#include <sys/types.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}


int main(int argc,char* argv[])
{
    int j;
    if(argc > 1 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s [msqid...]\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    for(j = 1;j < argc;j++)
    {
        if(msgctl(atoi(argv[j]),IPC_RMID,NULL) == -1)
        {
            errExit("msgctl()");
        }
    }

    exit(EXIT_SUCCESS);
}