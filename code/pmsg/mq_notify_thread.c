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
#include <pthread.h>
#include <mqueue.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>


static void notifySetup(mqd_t *mqdp);
static void threadFunc(union sigval sv)      /* Thread notification function */
{
    ssize_t numRead;
    mqd_t *mqdp;
    void *buffer;
    struct mq_attr attr;

    mqdp = (mqd_t *)(sv.sival_ptr);

    /* Determine mq_msgsize for message queue, and allocate an input buffer
       of that size */

    if (mq_getattr(*mqdp, &attr) == -1){
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }


    buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }


    /* Reregister for message notification */
    // 当消息通知发生时，程序会在清空队列之前重新启用通知
    notifySetup(mqdp);

    while ((numRead = mq_receive(*mqdp, (char *)buffer, attr.mq_msgsize, NULL)) >= 0)
        printf("Read %ld bytes\n", (long) numRead);
    /*FIXME: above: should use %zd here, and remove (long) cast */

    if (errno != EAGAIN)                        /* Unexpected error */
    {
        printf("mq_receive");
        exit(EXIT_FAILURE);
    }

    free(buffer);
}
static void notifySetup(mqd_t *mqdp)
{
    struct sigevent sev;

    sev.sigev_notify = SIGEV_THREAD;            //程序通过一个线程来请求通知需要将传入 mq_notify()的 sigevent 结构的 sigev_notify字段的值指定为 SIGEV_THREAD 。
    sev.sigev_notify_function = threadFunc;    //线程的启动函数 threadFunc() 是通过sigev_notify_function 字段来指定的
    sev.sigev_notify_attributes = NULL;
    /* Could be pointer to pthread_attr_t structure */
    sev.sigev_value.sival_ptr = mqdp;           /* Argument to threadFunc() */

    if (mq_notify(*mqdp, &sev) == -1){
        printf("mq_notify");
        exit(EXIT_FAILURE);
    }

}
int main(int argc, char *argv[])
{
    mqd_t mqd;

    if (argc != 2 || strcmp(argv[1], "--help") == 0){
        printf("%s mq-name\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // 采用了非阻塞模式使得在接收到一个通知之后可以在无需阻塞的情况下完全清空队列
    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1){
        printf("mq_open");
        exit(EXIT_FAILURE);
    }


    notifySetup(&mqd);
    pause();                    /* Wait for notifications via thread function */
    // 在启用消息通知之后，主程序会永远中止；定时器通知是通过在一个单独的线程中调用 threadFunc()来分发的①。
}