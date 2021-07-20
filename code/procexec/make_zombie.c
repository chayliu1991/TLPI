#include <unistd.h>
#include <signal.h>
#include <libgen.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define CMD_SIZE (200)

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    char cmd[CMD_SIZE];
    pid_t childPid;

    setbuf(stdout,NULL);
    printf("Parend PID=%ld\n",(long)getpid());

    switch(childPid = fork())
    {
        case -1:
            errExit("fork()");
        case 0:
            printf("Child (PID=%ld) exiting\n",(long)getpid());
            exit(EXIT_SUCCESS);
        default:
            sleep(3);
            snprintf(cmd,CMD_SIZE,"ps | grep %s",basename(argv[0]));
            cmd[CMD_SIZE-1] = '\0';
            system(cmd);

            if(kill(childPid,SIGKILL) == -1)
                printf("err kill\n");
            
            sleep(3);
            printf("After sending SIGKILL to zombie (PID=%ld)\n",(long)getpid());
            system(cmd);

            exit(EXIT_SUCCESS);  
    }      
}
