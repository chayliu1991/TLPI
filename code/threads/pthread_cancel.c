#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

static void* threadFunc(void* arg)
{
   int j;
   printf("New thread staeted\n");

   for(j = 1;;j++)
   {
       printf("Loop %d\n",j);
       sleep(1);
   }
   return NULL;
}

int main(int argc,char* argv[])
{
    pthread_t thr;
    int s;
    void *res;

    s = pthread_create(&thr,NULL,threadFunc,NULL);
    if(s != 0)
        errExit("pthread_create()");
    
    sleep(2);

    s = pthread_cancel(thr);
    if (s != 0)
        errExit("pthread_cancel()");

    s = pthread_join(thr,&res);
    if (s != 0)
        errExit("pthread_join()");
    
    if(res == PTHREAD_CANCELED)
        printf("Thread was canceled\n");
    else
        printf("Thread was not canceled(should not happen!)\n");

    exit(EXIT_SUCCESS);
}