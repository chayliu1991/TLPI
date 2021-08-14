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

        numBytes = recvfrom(sfd,resp,BUF_SIZE,0,NULL,NULL);
        if(numBytes == -1)
            errExit("recvfrom()");

        printf("Respone %d : %.*s\n",j-1,(int)numBytes,resp);
    }

    exit(EXIT_SUCCESS);
}