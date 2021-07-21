#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    printf("Initial value of USER: %s\n",getenv("USER"));
    if(putenv("USER=britta") != 0)
    {
        errExit("putenv");
    }

    execl("/usr/bin/printenv","printenv","USER","SHELL",(char*)NULL);
    errExit("execl"); 
}
