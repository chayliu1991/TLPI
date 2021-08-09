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

int main(int argc,char* argv[])
{
    mqd_t mqd;
    struct mq_attr attr;
    
    if(argc !=2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s mq-name\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    mqd = mq_open(argv[1],O_RDONLY);
    if(mqd == (mqd_t) - 1)
        errExit("mq_open()");
    
    if(mq_getattr(mqd,&attr) == -1)
        errExit("mq_getattr()");
    
    printf("Maximum # of message on queue： %ld \n",attr.mq_maxmsg);
    printf("Maximum message size： %ld \n",attr.mq_maxmsg);
    printf("# of messages currently on queue ： %ld \n",attr.mq_curmsgs);

    exit(EXIT_SUCCESS);
}