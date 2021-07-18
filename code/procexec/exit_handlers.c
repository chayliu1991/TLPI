#define _DEFAULT_SOURCE

#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

static void atexitFun1()
{
    printf("atexitFun1 called \n");
}

static void atexitFun2()
{
    printf("atexitFun2 called \n");
}

static void atexitFun3()
{
   printf("atexitFun3 called \n");
}

static void onExitFunc(int exitStatus,void* arg)
{
    printf("on_exit function called:status:%d,arg=%ld\n",exitStatus,(long)arg);
}

int main(int argc,char* argv)
{
     if(on_exit(onExitFunc,(void*)10) != 0)
    {
        errExit("on_exit()");
    }

    if(atexit(atexitFun1) != 0)
    {
        errExit("atexit()");
    }

    if(atexit(atexitFun2) != 0)
    {
        errExit("atexit()");
    }

    if(atexit(atexitFun3) != 0)
    {
        errExit("atexit()");
    }

    printf("main finish\n");
  
    exit(EXIT_SUCCESS);    
}
