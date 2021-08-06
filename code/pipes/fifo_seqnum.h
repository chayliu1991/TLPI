#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define SERVER_FIFO ("/tmp/seqnum_sv")
#define CLIENT_FIFO_TEMPLATE ("/tmp/seqnum_cl.%ld")
#define CLIENT_FIFO_NAME_LEN (sizeof(CLIENT_FIFO_TEMPLATE) + 20)

struct request
{
    pid_t pid;
    int seqLen;
};

struct response
{
    int seqNum;
};
