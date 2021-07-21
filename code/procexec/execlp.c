#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    if(argc !=2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s pathname\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    execlp(argv[1],argv[1],"hello world",(char*)NULL);

    errExit("execlp");    
}
