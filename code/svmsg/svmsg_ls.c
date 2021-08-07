#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

int errExit(char* caller)
{
    perror(caller);
    exit(1);
}

int main(int argc,char* argv[])
{
    int maxind,ind,msqid;
    struct msqid_ds ds;
    struct msginfo msginfo;

    maxind = msgctl(0,MSG_INFO,(struct msqid_ds*)&msginfo);
    if(maxind == -1)
        errExit("msgctl()");

    printf("maxind : %d \n\n",maxind);
    printf("index      id      key      message\n");

    for(ind = 0;ind <= maxind;ind++)
    {
        msqid = msgctl(ind,MSG_STAT,&ds);
        if(msqid == -1)
        {
            if(errno != EINVAL && errno != EACCES)
                fprintf(stderr,"msgctl - MSG_STAT");
            continue;
        }


        printf("%4d %8d 0x%0dlx %7ld\n",ind,msqid,(unsigned long)ds.msg_perm.__key,(long)ds.msg_qnum);
    }


    exit(EXIT_SUCCESS);
}
