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
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];

    if(argc < 3 | strcmp(argv[1],"--help") == 0)
    {
        printf("%s host-address msg...",argv[0]);
        exit(EXIT_SUCCESS);
    }

    sfd = socket(AF_INET6,SOCK_DGRAM,0);
    if(sfd ==-1)
        errExit("socket()");

    memset(&svaddr,0,sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6;
    svaddr.sin6_port = htons(PROT_NUM);
    if(inet_pton(AF_INET6,argv[1],&svaddr.sin6_addr) <= 0)
        errExit("inet_pton()");

    for (j = 2; j < argc;j++)
    {
        msgLen = strlen(argv[j]);
        if (sendto(sfd,argv[j],msgLen,0,(struct sockaddr*)&svaddr,sizeof(struct sockaddr_in6)) != msgLen)
            errExit("sendto()");

        numBytes = recvfrom();
    }

    // if(bind(sfd,(struct sockaddr_in6*)&svaddr,sizeof(struct sockaddr_in6)) == -1)
    //     errExit("bind()");

    // for (;;)
    // {
    //     len = sizeof(struct sockaddr_in6);
    //     numBytes = recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr*)&claddr,&len);
    //     if(numBytes == -1)
    //         errExit("recvfrom()");
    //     if (inet_ntop(AF_INET6, &claddr.sin6_addr, claddrStr,INET6_ADDRSTRLEN) == NULL)
    //         printf("could not convert client address to string\n");
    //     else
    //         printf("Sever received %ld bytes from (%s,%u)\n",(long)numBytes,claddr,ntohs(claddr.sin6_port));
        
    //     if(sendto(sfd,buf,numBytes,0,(struct sockaddr*)&claddr,len) != numBytes)
    //         errExit("sendto()");
    // }
}