#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>



#define KEY_FILE "/some-path/some-file"

void errExit(const char* call)
{
    perror(call);
    exit(EXIT_FAILURE);
}

int main(int argc,char* argv)
{
    int msqid;
    key_t key;

    const int MQ_PERMS = S_IRUSR | S_IWUSR | S_IWGRP;

    /* Optional code here to check if another server process is already running */

    key = ftok(KEY_FILE,1);
    if(key == -1)
        errExit("ftok()");

    
    while((msqid = msgget(key,IPC_CREAT | IPC_EXCL | MQ_PERMS)) == -1)
    {
        if(errno == EEXIST)
        {
            msqid = msgget(key,0);
            if(msqid == -1)
                errExit("msgget() failed to retrieve old queue ID");
            if(msgctl(msqid,IPC_RMID,NULL) == -1)
                errExit("msgctl() failed to delete old queue");
        }
        else{
            errExit("msgget() failed");
        }
    }

    /* Upon loop exit,we're successfully created the message queue,and we can then carry on to do other work*/

    exit(EXIT_SUCCESS);
}