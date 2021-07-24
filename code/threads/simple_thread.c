#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}


static void* threadFunc(void* arg)
{
    char* s = (char*)arg;
    printf("%s",s);
    return (void*) strlen(s); 
}

int main(int argc,char* argv[])
{
    int s ;
    pthread_t t1;
    void* res;

    s = pthread_create(&t1,NULL,threadFunc,"Hello,World\n");
    if(s != 0)
        errExit("pthread_create()");
    
    printf("Message from main()\n");

    s = pthread_join(t1,&res);
    if(s != 0)
         errExit("pthread_join()");
    
    printf("Thread returned %ld\n",(long)res);

    exit(EXIT_SUCCESS);
}
