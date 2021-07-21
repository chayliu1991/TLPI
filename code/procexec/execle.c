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
    char* envVec[] = {"GREET=salut","BYE=adieu",NULL};
    char* filename;
    
    if(argc !=2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s pathname\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    filename = strrchr(argv[1],'/');
    if(filename != NULL)
        filename++;
    else
        filename = argv[1];

    execle(argv[1],filename,"hello world",(char*)NULL,envVec);

    errExit("execle");    
}
