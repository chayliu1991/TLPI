#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


#define MAX_ERRORLEN 256

static char buf[MAX_ERRORLEN];

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

char* strerror(int err)
{
    if(err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL)
        snprintf(buf,MAX_ERRORLEN,"Unknown error :%d\n",err);
    else
    {
        strncpy(buf,_sys_errlist[err],MAX_ERRORLEN-1);
        buf[MAX_ERRORLEN-1] = '\0';
    }

    return buf;
}

static void* threadFunc(void* arg)
{
    char* str;
    printf("Other thread about to call strerror()\n");
    str = strerror(EPERM);
    printf("Other thread:str(%p) = %s\n",str,str);
    return NULL;
}

int main(int argc,char* argv[])
{
    pthread_t t;
    int s;
    char* str;


    str = strerror(EINVAL);
    printf("Main thread has called strerror()\n");

    s = pthread_create(&t,NULL,threadFunc,NULL);
    if(s != 0)
    {
        errExit("pthread_create()");
    }

    s  = pthread_join(t,NULL);
    if(s != 0)
    {
        errExit("pthread_join()");
    }

    printf("Main thread : str(%p) = %s\n",str,str);

    exit(EXIT_SUCCESS);
}