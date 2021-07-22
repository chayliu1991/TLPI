#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

int system(char* command)
{
    sigset_t blockMask,oriMask;
    struct sigaction saIgnore,saOrigQuit,saOrigInt,saDefault;
    pid_t childPid;
    int status,saveErrno;


    if(command == NULL)
        return system(":") == 0;
    
    sigemptyset(&blockMask)
    sigaddset(&blockMask,SIGCHLD);
    sigprocmask(SIG_BLOCK,&blockMask,&oriMask);

    saIgnore.sa_handler = SIG_IGN;
    saIgnore.sa_flags = 0;
    sigemptyset(&saIgnore.sa_mask);
    sigaction(SIGINT,&saIgnore,&saOrigInt);
    sigaction(SIGQUIT,&saIgnore,&saOrigInt);

    switch(childPid = fork())
    {
        case -1:
            status = -1; 
            break;
        case 0:
            saDefault,sa_handler = SIG_DFL;
            saDefault.sa_flags = 0;
            sigemptyset(&saDefault.sa_mask);

            if(saOrigInt.sa_handler != SIG_IGN)
                sigaction(SIGINT,&saDefault,NULL);

            if(saOrigQuit.sa_handler != SIG_IGN)
                sigaction(SIGQUIT,&saDefault,NULL);

            sigprocmask(SIG_SETMASK,&oriMask,NULL);
            exel("/bin/sh","sh","-c",command,(char*)NULL);
            _exit(127);
        default:
            while(waitpid(childPid,&status,0) == -1)
            {
                if(errno != EINTR)
                {
                    status = -1;
                    break;
                }
            }
            break;
    }

    saveErrno = errno;
    sigprocmask(SIG_SETMASK,&oriMask,NULL);
    sigprocmask(SIG_SETMASK,&oriMask,NULL);
    sigprocmask(SIG_SETMASK,&oriMask,NULL);
  
}
