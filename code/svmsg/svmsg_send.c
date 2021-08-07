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

void cmdLineErr(const char *format, ...)
{
    va_list argList;

    fflush(stdout); 

    fprintf(stderr, "Command-line usage error: ");
    va_start(argList, format);
    vfprintf(stderr, format, argList);
    va_end(argList);

    fflush(stderr); 
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[])
{
    int msqid, flags, msgLen;
    struct mbuf msg; 
    int opt;

    flags = 0;
    while ((opt = getopt(argc, argv, "n")) != -1)
    {
        if (opt == 'n')
            flags |= IPC_NOWAIT;
        else
            usageError(argv[0], NULL);
    }

    if (argc < optind + 2 || argc > optind + 3)
        usageError(argv[0], "Wrong number of arguments\n");

    msqid = atoi(argv[optind]);
    msg.mtype = atoi(argv[optind + 1]);

    if (argc > optind + 2) 
    {
        msgLen = strlen(argv[optind + 2]) + 1;
        if (msgLen > MAX_MTEXT)
            cmdLineErr("msg-text too long (max: %d characters)\n", MAX_MTEXT);

        memcpy(msg.mtext, argv[optind + 2], msgLen);

    } else {  
        msgLen = 0;
    }

    if (msgsnd(msqid, &msg, msgLen, flags) == -1){
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
        
    exit(EXIT_SUCCESS);
}