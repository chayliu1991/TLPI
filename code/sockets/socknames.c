#include "inet_sockets.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}


int main(int argc,char* argv[])
{
    int listenFd, acceptFd, connFd;
    socklen_t len;
    void *addr;
    char addStr[IS_ADDR_STR_LEN];

    if(argc !=2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s service \n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    connFd = inetConnect(NULL,argv[1],SOCK_STREAM);
    if(connFd == -1)
        errExit("inetConnect()");

    acceptFd = accept(listenFd,NULL,NULL);
    if(acceptFd == -1)
        errExit("acceptFd()");

    addr = malloc(len);
    if(addr == NULL)
        errExit("malloc()");
    
    if(getsockname(connFd,addr,&len) == -1)
        errExit("getsockname()");

    printf("getsockname(connFd): %s\n",inetAddressStr(addr,len,addStr,IS_ADDR_STR_LEN));

    if(getsockname(acceptFd,addr,&len) == -1)
        errExit("getsockname()");

    printf("getsockname(acceptFd): %s\n",inetAddressStr(addr,len,addStr,IS_ADDR_STR_LEN));

    if(getpeername(connFd,addr,&len) == -1)
        errExit("getpeername()");
    printf("getpeername(connFd): %s\n",inetAddressStr(addr,len,addStr,IS_ADDR_STR_LEN));

    if(getpeername(acceptFd,addr,&len) == -1)
        errExit("getpeername()");
    printf("getpeername(acceptFd): %s\n",inetAddressStr(addr,len,addStr,IS_ADDR_STR_LEN));

    sleep(30);
    exit(EXIT_SUCCESS);
}