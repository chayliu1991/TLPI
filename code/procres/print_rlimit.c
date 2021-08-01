#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int printRlimit(const char* msg,int resource)
{
    struct rlimit rlim;
    if(getrlimit(resource,&rlim)== -1)
    {
        return -1;
    }

    printf("%s soft=",msg);

    if(rlim.rlim_cur == RLIM_INFINITY)
        printf("infinite");
    #ifdef RLIM_SAVED_CUR
        else if(rlim.rlim_cur == RLIM_SAVED_CUR)
            printf("unrepresentable");
    #endif
    else
         printf("%lld",(long long)rlim.rlim_cur);

    printf(";hard=");
    if(rlim.rlim_max == RLIM_INFINITY)
        printf("infinite");
    #ifdef RLIM_SAVED_MAX
        else if(rlim.rlim_max == RLIM_SAVED_MAX)
            printf("unrepresentable");
     #endif
     else
        printf("%lld\n",(long long)rlim.rlim_max);
    
    return 0;
}

int main(int argc,char* argv[])
{
    struct rlimit rl;
    int j;
    pid_t childPid;

    if(argc < 2 || argc < 3 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s soft-limit [hard-limit]\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    rl.rlim_cur = (argv[1][0] == 'i') ? RLIM_INFINITY : atoi(argv[1]);
    rl.rlim_max = (argc == 2) ? rl.rlim_cur : (argv[2][0] == 'i') ? RLIM_INFINITY : atoi(argv[2]);

    if(setrlimit(RLIMIT_NPROC,&rl) == -1)
    {
        errExit("setrlimit()");
    }

    for(j =1;;j++)
    {
        switch(childPid = fork())
        {
            case -1:
                errExit("fork()");
            case 0:
                _exit(EXIT_SUCCESS);
            default:
                printf("Child %d (PID=%ld) started \n",j,(long)childPid);
                break;
        }
    }
}