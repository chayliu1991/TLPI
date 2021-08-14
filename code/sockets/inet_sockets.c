#define _BSD_SOURCE

#include "inet_sockets.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


int inetConnect(const char *host, const char *service, int type)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s;

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = type;
    hints.ai_family = AF_UNSPEC;

    s = getaddrinfo(host, service, &hints, &result);
    if(s != 0)
    {
        errno = ENOSYS;
        return -1;
    }
    
    for (rp = result; rp != NULL;rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
        if(sfd == -1)
            continue;
          
        if(connect(sfd,rp->ai_addr,rp->ai_addrlen) != -1)
            break;

        close(sfd);    
    }

    freeaddrinfo(result);

    return rp == NULL ? -1 : sfd;
}

static int inetPassiveSocket(const char* service,int type,socklen_t* addrLen,Boolean doListen,int backlog)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd, s, optval;

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = type;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, service, &hints, &result);
    if(s != 0)
    {
        return -1;
    }

    optval = 1;

    for (rp = result; rp != NULL;rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
        if(sfd == -1)
            continue;
        
        if(doListen)
        {
            if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval)) == -1)
            {
                close(sfd);
                freeaddrinfo(result);
                return -1;
            }
        }

        if(bind(sfd,rp->ai_addr,rp->ai_addrlen) ==0)
            break;
        close(sfd);    
    }

    if(rp != NULL && addrLen != NULL)
    {
        *addrLen = rp->ai_addrlen;
    }
    freeaddrinfo(result);
    return rp == NULL ? -1 : sfd;
}


int inetListen(const char *service, int backlog, socklen_t *addrlen)
{
    return inetPassiveSocket(service,SOCK_STREAM,addrlen,True,backlog);
}

int inetBind(const char *service, int type, socklen_t *addrlen)
{
    return inetPassiveSocket(service,type,addrlen,False,0);
}

char *inetAddressStr(const struct sockaddr *addr, socklen_t addrLen, char *addrStr, int addrStrLen)
{
    char host[NI_MAXHOST], service[NI_MAXSERV];
    if(getnameinfo(addr,addrLen,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICSERV) == 0)
        snprintf(addrStr,addrStrLen,("%s,%s"),host,service);
    else
        snprintf(addrStr,addrStrLen,"?UNKNOWN?");

    addrStr[addrLen - 1] = '\0';
    return addrStr;
}
