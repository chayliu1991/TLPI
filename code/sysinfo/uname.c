#define _GNU_SOURCE

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

int main()
{
    struct utsname uts;
    if(uname(&uts) == -1)
        errExit("uname");

    printf("Node name:  %s\n",uts.nodename);
    printf("System name:  %s\n",uts.sysname);
    printf("Release :  %s\n",uts.release);    
    printf("Version :  %s\n",uts.version);
    printf("Machine :  %s\n",uts.machine);

    #ifdef _GNU_SOURCE
    printf("Domain name:  %s\n",uts.domainname);
    #endif

    exit(EXIT_SUCCESS);
}


