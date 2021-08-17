#include <time.h>
#include <poll.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int errExit(char* caller)
{
    perror(caller);
    exit(1);
}


int main(int argc, char *argv[])
{
    int numPipes, j, ready, randPipe, numWrites;
    int (*pfds)[2];   
    struct pollfd *pollFd;

    if (argc < 2 || strcmp(argv[1], "--help") == 0)
    {
        printf("%s num-pipes [num-writes]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }        

    numPipes = atoi(argv[1]);
    pfds = calloc(numPipes, sizeof(int [2]));
    if (pfds == NULL)
        errExit("calloc");
    pollFd = calloc(numPipes, sizeof(struct pollfd));
    if (pollFd == NULL)
        errExit("calloc");

    for (j = 0; j < numPipes; j++)
        if (pipe(pfds[j]) == -1)
        {
            printf("pipe %d", j);
            exit(EXIT_FAILURE);
        }
            

    numWrites = (argc > 2) ? atoi(argv[2]) : 1;

    srandom((int) time(NULL));
    for (j = 0; j < numWrites; j++) 
    {
        randPipe = random() % numPipes;
        printf("Writing to fd: %3d (read fd: %3d)\n", pfds[randPipe][1], pfds[randPipe][0]);
        if (write(pfds[randPipe][1], "a", 1) == -1)
        {
            printf("write %d", pfds[randPipe][1]);
            exit(EXIT_FAILURE);
        }
    }

    for (j = 0; j < numPipes; j++)
    {
        pollFd[j].fd = pfds[j][0];
        pollFd[j].events = POLLIN;
    }

    ready = poll(pollFd, numPipes, 0);
    if (ready == -1)
        errExit("poll");

    printf("poll() returned: %d\n", ready);

    for (j = 0; j < numPipes; j++)
        if (pollFd[j].revents & POLLIN)
            printf("Readable: %3d\n", pollFd[j].fd);

    exit(EXIT_SUCCESS);
}
