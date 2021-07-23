#define _DEFAULT_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}


int main(int argc,char* argv[])
{
    if(argc  <= 1 || argc > 2|| (argc > 1 && strcmp(argv[1],"--help")== 0))
    {
        printf("%s [file]\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    if(acct(argv[1]) == -1)
        errExit("acct()");
    
    printf("Process accounting %s\n",(argv[1] == NULL ? "disabled" : "enabled"));

    exit(EXIT_SUCCESS);
}