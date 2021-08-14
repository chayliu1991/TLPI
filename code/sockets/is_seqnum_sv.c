#define _BSD_SOURCE

#include <netdb.h>

#include "is_seqnum.h"
#include "read_line.h"

#define BACKLOG 50

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    uint32_t seqNum;
    char reqLenStr[INT_LEN];
    char seqNumStr[INT_LEN];
    struct sockaddr_storage claddr;
    int lfd, cfd, optval, reqLen;
    socklen_t addrlen;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    #define ADDRSTRLEN (NI_MAXHOST + NI_MAXSERV +10)

    char addrStr[ADDRSTRLEN];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    if(argc  > 1 && strcmp(argv[1],"--help") == 0)
    {
        printf("%s [init-seq-num]\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    seqNum = (argc > 1) ? atoi(argv[1]) : 0;
    if(signal(SIGPIPE,SIG_IGN) == SIG_ERR)
        errExit("signal()");

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE | AI_NUMERICSERV;

    if(getaddrinfo(NULL,PORT_NUM,&hints,&result) != 0)
        errExit("getaddrinfo()");

    optval = 1;
    for (rp = result; rp != NULL;rp = rp->ai_next)
    {
        lfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
        if(lfd == -1)
            continue;
        
        if(setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval)) == -1)
            errExit("setsockopt()");
        
        if(bind(lfd,rp->ai_addr,rp->ai_addrlen) == 0)
            break;

        close(lfd);
    }

    if(rp == NULL)
        errExit("could not bind socket any address");
    
    if(listen(lfd,BACKLOG) == -1)
        errExit("listen()");

    freeaddrinfo(result);

    for (;;)
    {
        addrlen = sizeof(struct sockaddr_storage);
        cfd = accept(lfd,(struct sockaddr*)&claddr,&addrlen);
        if(cfd == -1)
        {
            printf("accept\n");
            continue;
        }

        if(getnameinfo((struct sockaddr*)&claddr,addrlen,host,NI_MAXHOST,service,NI_MAXSERV,0) == 0)
            snprintf(addrStr,ADDRSTRLEN,"(%s,%s)",host,service);
        else
            snprintf(addrStr,ADDRSTRLEN,"(?UNKNOWN?)");
        
        if(readLine(cfd,reqLenStr,INT_LEN) <= 0)
        {
            close(cfd);
            continue;
        }

        snprintf(seqNumStr,INT_LEN,"%d\n",seqNum);
        if(write(cfd,&seqNumStr,strlen(seqNumStr)) != strlen(seqNumStr))
            printf("Error on write\n");

        seqNum += reqLen;
        if(close(cfd) == -1)
            printf("Error on close");
    }
}