#include <mqueue.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>


void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

static void usageError(const char* progName)
{
    fprintf(stderr,"Usage: %s [-cx] [-m maxmsg] [-s msgsize] mq-name [octal-perms] \n",progName);
    fprintf(stderr,"        -c              Create queue (O_CREAT)\n");
    fprintf(stderr,"        -m maxmsg       Set maximum # of messages\n");
    fprintf(stderr,"        -s msgsize      Set maximum message size\n");
    fprintf(stderr,"        -x              Create exclusively (O_EXCL)\n");
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv[])
{
    int flags, opt;
    mode_t perms;
    mqd_t mqd;
    struct mq_attr attr, *attrp;

    attrp = NULL;
    attr.mq_maxmsg = 50;
    attr.mq_msgsize = 2048;
    flags = O_RDWR;

    while((opt = getopt(argc,argv,"cm:s:x")) !=-1)
    {
        switch (opt)
        {
        case /* constant-expression */:
            /* code */
            break;
        
        default:
            break;
        }
    }

    exit(EXIT_SUCCESS);
}