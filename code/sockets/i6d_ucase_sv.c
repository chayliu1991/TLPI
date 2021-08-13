#include "i6d_ucase.h"

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    struct sockaddr_in6 svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    char claddrStr[INET_ADDRSTRLEN];

    sfd = socket(AF_INET6,SOCK_DGRAM,0);
    if(sfd ==-1)
        errExit("socket()");

    memset(&svaddr,0,sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6;
    svaddr.sin6_addr = in6addr_any;
    svaddr.sin6_port = htons(PROT_NUM);

    if(bind(sfd,(struct sockaddr_in6*)&svaddr,sizeof(struct sockaddr_in6)) == -1)
        errExit("bind()");

    for (;;)
    {
        len = sizeof(struct sockaddr_in6);
        numBytes = recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr*)&claddr,&len);
        if(numBytes == -1)
            errExit("recvfrom()");
        if (inet_ntop(AF_INET6, &claddr.sin6_addr, claddrStr,INET6_ADDRSTRLEN) == NULL)
            printf("could not convert client address to string\n");
        else
            printf("Sever received %ld bytes from (%s,%u)\n",(long)numBytes,claddr,ntohs(claddr.sin6_port));
        
        if(sendto(sfd,buf,numBytes,0,(struct sockaddr*)&claddr,len) != numBytes)
            errExit("sendto()");
    }
}