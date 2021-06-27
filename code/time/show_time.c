#include <time.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
 #include <stdio.h>

#define BUF_SIZE 200

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

int main(int argc,char* argv[])
{
    time_t t;
    struct tm* loc;
    char buf[BUF_SIZE];

    if(setlocale(LC_ALL,"") == NULL)
        errExit("setlocale");
    t = time(NULL);
    
    printf("ctime of time() value is:%s\n",ctime(&t));

    loc = localtime(&t);
    if(loc == NULL)
        errExit("localtime");
    printf("asctime() of local time is:%s\n",asctime(loc));

    if(strftime(buf,BUF_SIZE,"%A.%d,%B,%T,%H:%M:%S %Z",loc) == 0) 
        errExit("strftime");
    
    printf("strftime() of local time is %s\n",buf);
    

    exit(EXIT_SUCCESS);
}