#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <stddef.h> 
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>



#define SERVER_KEY 0x1aaaaaa1

struct requestMsg {
    long mtype;
    int  clientId;
    char pathname[PATH_MAX];
};


#define REQ_MSG_SIZE (offsetof(struct requestMsg, pathname) - \
                      offsetof(struct requestMsg, clientId) + PATH_MAX)

#define RESP_MSG_SIZE 8192

struct responseMsg {
    long mtype;
    char data[RESP_MSG_SIZE];
};


#define RESP_MT_FAILURE 1 
#define RESP_MT_DATA    2 
#define RESP_MT_END     3 
