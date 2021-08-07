#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>


#define MAX_MTEXT 1024

struct mbuf 
{
    long mtype;
    char mtext[MAX_MTEXT];
};

static void usageError(const char *progName, const char *msg)
{
    if (msg != NULL)
        fprintf(stderr, "%s", msg);
    fprintf(stderr, "Usage: %s [-n] msqid msg-type [msg-text]\n", progName);
    fprintf(stderr, "    -n       Use IPC_NOWAIT flag\n");
    exit(EXIT_FAILURE);
}


int errExit(char* caller)
{
    perror(caller);
    exit(1);
}


int main(int argc, char *argv[])
{
    int msqid,flags,type;
    ssize_t msgLen;
    size_t maxBytes;
    struct mbuf msg;
    int opt;

    flags = 0;
    type = 0;

    while ((opt = getopt(argc, argv, "ent:x")) != -1)
    {
        switch(opt)
        {
            case 'e':
                flags |= MSG_NOERROR;
                break;
            case 'n':
                flags |= IPC_NOWAIT;
                break;
            case 't':
                type = atoi(optarg);
                break;
            
            #ifdef MSG_EXCEPT
            case 'x':
                flags |= MSG_EXCEPT;
                break;
            #endif
            default:
                usageError(argv[0],NULL);
        }
    }

    if(argc <  optind + 1 || argc > optind +2)
    {
        usageError(argv[0],"Wrong number of arguments\n");
    } 

    msqid = atoi(argv[optind]);
    maxBytes = argc > (optind + 1) ? atoi(argv[optind+1]) : MAX_MTEXT;
    msgLen = msgrcv(msqid,&msg,maxBytes,type,flags);
    if(msgLen == -1)
        errExit("msgrcv");

    printf("Received : type = %ld;length = %ld",msg.mtype,(long)msgLen);
    if(msgLen > 0)
        printf(";body=%s",msg.mtext);
    
    printf("\n");

        
    exit(EXIT_SUCCESS);
}