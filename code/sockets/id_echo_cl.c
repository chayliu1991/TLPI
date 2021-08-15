#include "id_echo.h"

#include <syslog.h>

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    int sfd,j;
    size_t len;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s host msg...\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    sfd = inetConnect(argv[1],SERVICE,SOCK_DGRAM);
    if(sfd == -1)
        errExit("Colud not connect to server port");
    
    for(j = 2;j < argc;j++)
    {
        len = strlen(argv[j]);
        if(write(sfd,argv[j],len) != len)
            errExit("write()");
        
        numRead = read(sfd,buf,BUF_SIZE);
        if(numRead == -1)
            errExit("read()");
        
        printf("[%ld bytes] %.*s\n",(long)numRead,(int)numRead,buf);
    }
    
    exit(EXIT_SUCCESS);  
}