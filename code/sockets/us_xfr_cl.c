#include "us_xfr.h"

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    struct sockaddr_un addr;
    int sfd;
    ssize_t numRead;
    char buf[BUFSIZ];

    sfd = socket(AF_UNIX,SOCK_STREAM,0);
    if(sfd == -1)
        errExit("socket()");
    
    memset(&addr,0,sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,SV_SOCK_PATH,sizeof(addr.sun_path) - 1);


    if(connect(sfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un)) == -1)
        errExit("connect()");
    
    while((numRead = read(STDIN_FILENO,buf,BUFSIZ)) > 0)
    {
        if(write(sfd,buf,numRead) != numRead)
            errExit("write()");
    }

    if(numRead == -1)
        errExit("read()");
    
    exit(EXIT_SUCCESS);
}
