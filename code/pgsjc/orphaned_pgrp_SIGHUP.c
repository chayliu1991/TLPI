#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

static void handler(int sig)
{
   printf("PID =%ld : caught signal %d (%s)\n",(long)getpid(),sig,strsignal(sig));
}

int main(int argc,char* argv[])
{
    int j;
    struct sigaction sa;

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s {s|p} ... \n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    setbuf(stdout,NULL);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    if(sigaction(SIGHUP,&sa,NULL) == -1)
        errExit("sigaction()");

    if(sigaction(SIGCONT,&sa,NULL) == -1)
        errExit("sigaction()");

    printf("parent: PID = %ld,PPID = %ld,PGID = %ld,SID = %ld\n",(long)getpid(),(long)getppid(),(long)getpgrp(),(long)getsid(0));

    for(j = 1;j < argc;j++)
    {
        switch(fork())
        {
            case -1:
                errExit("fork()");
            case 0:
               printf("parent: PID = %ld,PPID = %ld,PGID = %ld,SID = %ld\n",(long)getpid(),(long)getppid(),(long)getpgrp(),(long)getsid(0));
               if(argv[j][0] == 's') 
               {
                   printf("PID = %ld pausing \n",(long)getpid());
                   raise(SIGSTOP);
               }
               else{
                   alarm(60);
                   printf("PID = %ld pausing \n",(long)getpid());
                   pause();
               }
               _exit(EXIT_SUCCESS);
            default:
                break;
        }
    }

    sleep(3);
    printf("parent exiting\n");
    exit(EXIT_SUCCESS);
}