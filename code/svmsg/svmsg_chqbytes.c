#include <sys/types.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}


int main(int argc,char* argv[])
{
    struct msqid_ds ds;
    int msqid;

    if(argc != 3 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s msqid max-bytes\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    msqid = atoi(argv[1]);
    if(msgctl(msqid,IPC_STAT,&ds) == -1)
        errExit("msgctl()");

    ds.msg_qbytes = atoi(argv[2]);

    if(msgctl(msqid,IPC_SET,&ds) == -1)
        errExit("msgctl()");

    exit(EXIT_SUCCESS);
}