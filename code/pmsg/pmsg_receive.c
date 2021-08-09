#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>

static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] mq-name\n", progName);
    fprintf(stderr, "    -n           Use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
    int flags, opt;
    mqd_t mqd;
    unsigned int prio;
    void *buffer;
    struct mq_attr attr;
    ssize_t numRead;

    flags = O_RDONLY;
    while ((opt = getopt(argc, argv, "n")) != -1) {
        switch (opt) {
            case 'n':   
                flags |= O_NONBLOCK;        
                break;
            default:    
                usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1){
        perror("mq_open");
        exit(EXIT_FAILURE);
    }


    /* We need to know the 'mq_msgsize' attribute of the queue in
       order to determine the size of the buffer for mq_receive() */

    if (mq_getattr(mqd, &attr) == -1){
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }


    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }


    numRead = mq_receive(mqd, (char *)buffer, attr.mq_msgsize, &prio);
    if (numRead == -1){
        perror("mq_receive");
        exit(EXIT_FAILURE);
    }


    printf("Read %ld bytes; priority = %u\n", (long) numRead, prio);
    /*FIXME: above: should use %zd here, and remove (long) cast */
    if (write(STDOUT_FILENO, buffer, numRead) == -1){
        perror("write");
        exit(EXIT_FAILURE);
    }

    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}