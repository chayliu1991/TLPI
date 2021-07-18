#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>

static int idata = 111;

void errExit(const char* call)
{
    perror(call);
    exit(1);
}

int main(int argc,char* argv)
{
    int istack = 222;
    pid_t pid;

    switch(pid = fork())
    {
        case -1:
            errExit("fork");
        case 0:
            idata *= 3;
            istack *= 3;
            break;
        default:
            sleep(3);
            break;
    }
    
    //@ both parent and child come here

    printf("PID:%ld %s idata=%d istack=%d\n",(long)getpid(),(pid == 0) ? "(child)" : "(parent)",idata,istack);

    exit(EXIT_SUCCESS);
}