#define _POSIX_C_SOURCE 199309L

#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


void errExit(const char* call)
{
    perror(call);
    exit(1);
}


int main(int argc,char* argv[])
{
    if(argc < 4 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s pid sig-num data [num-sigs] \n",argv[0]);
        exit(1);
    }

    int sig, numSigs, j, sigData;
    union sigval sv;

    printf("%s PID is %ld,UID is %ld\n",argv[0],(long)getpid(),(long)getuid());

    sig = atoi(argv[2]);
    sigData = atoi(argv[3]);
    numSigs = (argc > 4) ? atoi(argv[4]) : 1;

    for (j = 0; j < numSigs;j++)
    {
        sv.sival_int = sigData + j;
        if(sigqueue(atol(argv[1]),sig,sv) == -1)
        {
            errExit("sigqueue");
        }
    }

    exit(EXIT_SUCCESS);
}