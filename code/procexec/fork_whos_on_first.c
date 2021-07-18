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

int main(int argc,char* argv[])
{
    int numChildren,j;
    pid_t pid;

    if(argc > 1 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s [num-children]\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    numChildren = (argc > 1) ? atoi(argv[1]) :  1;
    setbuf(stdout,NULL);

    for(j = 0;j < numChildren;++j)
    {
        switch(pid = fork())
        {
            case -1:
                errExit("fork");
            case 0:
                printf("%d child\n",j);
                _exit(EXIT_SUCCESS);
            default:
                printf("%d parent\n",j);
                wait(NULL);
                break;
        }
    }
    exit(EXIT_SUCCESS);  
}