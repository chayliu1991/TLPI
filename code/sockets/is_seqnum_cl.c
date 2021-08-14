#define _BSD_SOURCE

#include <netdb.h>

#include "is_seqnum.h"
#include "read_line.h"


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    char *reqLenStr;
    char seqNumStr[INT_LEN];
    int cfd;
    ssize_t numRead;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s server-host [sequence-len]\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_NUMERICSERV;

    if(getaddrinfo(NULL,PORT_NUM,&hints,&result) != 0)
        errExit("getaddrinfo()");

    for (rp = result; rp != NULL;rp = rp->ai_next)
    {
        cfd = socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);
        if(cfd == -1)
            continue;
        
        if(connect(cfd,rp->ai_addr,rp->ai_addrlen) != -1)
            break;

        close(cfd);
    }

    if(rp == NULL)
        errExit("could not bind socket any address");
    
    freeaddrinfo(result);

    reqLenStr = (argc > 2) ? argv[2] : "1";
    if(write(cfd,reqLenStr,strlen(reqLenStr)) != strlen(reqLenStr))
        errExit("write()");
    
    if(write(cfd,"\n",1) != 1)
        errExit("write()");

    numRead = readLine(cfd, seqNumStr, INT_LEN);
    if(numRead == -1)
        errExit("readLine()");
    if(numRead == 0)
        errExit("Unexpected EOF from server");

    printf("Sequence number: %s\n",seqNumStr);
    exit(EXIT_SUCCESS);    
}