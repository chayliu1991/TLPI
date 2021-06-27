#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 1000

char* current_time(const char* format)
{
    static char buf[BUF_SIZE];
    time_t t;
    size_t s;
    struct tm * tm;

    t = time(NULL);
    tm = localtime(&t);
    if(tm == NULL)
        return NULL;
    
    s = strftime(buf,BUF_SIZE,(format != NULL) ? format : "%c",tm);
    return s == 0 ? NULL : buf;
}

int main()
{
    char* curr = current_time(NULL);
    if(curr == NULL)
    {
        printf("can not get current time\n");
    }

    printf("current:%s\n",curr);

    return 0;
}