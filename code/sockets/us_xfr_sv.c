#include "us_xfr.h"


#define BACKLOG 5

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}


int main(int argc,char* argv[])
{
    struct sockaddr_un addr;
    int sfd,cfd;
    ssize_t numRead;
    char buf[BUFSIZ];


    sfd = socket(AF_UNIX,SOCK_STREAM,0);
    if(sfd == -1)
        errExit("socket()");
    
    if(remove(SV_SOCK_PATH) == -1 && errno != ENOENT)  //@ 移除所有的既有文件
        errExit("remove()");
    
    memset(&addr,0,sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,SV_SOCK_PATH,sizeof(addr.sun_path)-1);
    if(bind(sfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un)) == -1)
        errExit("bind()");

    if(listen(sfd,BACKLOG) == -1)
        errExit("listen()");

    for(;;)
    {   
        cfd = accept(sfd,NULL,NULL);
        if(cfd == -1)
            errExit("accept()");
        
        while((numRead = read(cfd,buf,BUFSIZ)) > 0)
        {
            if(write(STDOUT_FILENO,buf,numRead) != numRead)
                errExit("write()");
        }

        if(numRead == -1)
             errExit("read()");

        if(close(cfd) == -1)
             errExit("close()");
    }
}