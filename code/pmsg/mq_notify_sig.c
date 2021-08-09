#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>

#define NOTIFY_SIG SIGUSR1


static void handler(int sig)
{
    /* Just interrupt sigsuspend() */
}


/* This program does not handle the case where a message already exists on
   the queue by the time the first attempt is made to register for message
   notification. In that case, the program would never receive a notification.
   See mq_notify_via_signal.c for an example of how to deal with that case. */

int main(int argc, char *argv[])
{
    struct sigevent sev;
    mqd_t mqd;
    struct mq_attr attr;
    void *buffer;
    ssize_t numRead;
    sigset_t blockMask, emptyMask;
    struct sigaction sa;

    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        printf("%s mq-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 以非阻塞模式打开了一个通过命令行指定名称的消息队列
    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1){
        perror("mq_open");
        exit(EXIT_FAILURE);
    }

    // 确定该队列的mq_msgsize 特性的值
    if (mq_getattr(mqd, &attr) == -1){
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }

	// 分配一个大小为该值的缓冲区来接收消息
    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }


	// 阻塞通知信号（SIGUSR1）并为其建立一个处理器
    sigemptyset(&blockMask);
    sigaddset(&blockMask, NOTIFY_SIG);
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1){
        perror("sigprocmask");
        exit(EXIT_FAILURE);
    }


    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(NOTIFY_SIG, &sa, NULL) == -1){
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

	// 首次调用 mq_notify()来注册进程接收消息通知
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    if (mq_notify(mqd, &sev) == -1){
        perror("mq_notify");
        exit(EXIT_FAILURE);
    }


    sigemptyset(&emptyMask);

    for (;;) {
       //sigsuspend()解除通知信号的阻塞状态并等待直到信号被捕获。从这个系统调用
       //中返回表示已经发生了一个消息通知。此刻，进程会撤销消息通知的注册信息。
        sigsuspend(&emptyMask);         /* Wait for notification signal */

        /* Reregister for message notification */
		// 调用 mq_notify()重新注册进程接收消息通知
        if (mq_notify(mqd, &sev) == -1){
            perror("mq_notify");
            exit(EXIT_FAILURE);
        }

		//执行一个 while 循环从队列中尽可能多地读取消息以便清空队列
        while ((numRead = mq_receive(mqd, (char *)buffer, attr.mq_msgsize, NULL)) >= 0)
            printf("Read %ld bytes\n", (long) numRead);
        /*FIXME: above: should use %zd here, and remove (long) cast */

        if (errno != EAGAIN)            /* Unexpected error */
        {
            perror("mq_receive");
            exit(EXIT_FAILURE);
        }
    }
}