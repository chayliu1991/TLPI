#define _XOPEN_SOURCE

#include <time.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
 #include <stdio.h>

#define SBUF_SIZE 1000

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

int main(int argc,char* argv[])
{
    struct tm tm;
    char sbuf[SBUF_SIZE];
    char* ofmt;

    if (argc < 3 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s input-date-time in-format [out-format]\n",argv[0]);
        exit(0);
    }
    
    if(setlocale(LC_ALL,"") == NULL)
        errExit("setlocale");
    
    memset(&tm,0,sizeof(struct tm));
    if(strptime(argv[1],argv[2],&tm) == NULL)
        errExit("strptime");
    
    tm.tm_isdst = -1;

    printf("calendar time (seconds since Epoch):%ld\n",(long)mktime(&tm));
    ofmt = (argc < 3) ? argv[3] : "%H:%M:%S %A,%d %B %Y %Z";
    if(strftime(sbuf,SBUF_SIZE,ofmt,&tm) == 0)
        errExit("strftime");
    
    printf("strftime() yield:%s\n",sbuf);

    exit(EXIT_SUCCESS);

    return 0;
}

//./strtime 2019-10-22 12.22.09 "%c"
