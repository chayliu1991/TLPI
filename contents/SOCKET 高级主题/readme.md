# 流式套接字上的部分读和部分写

如果套接字上可用的数据比在 `read()` 调用中请求的数据要少，那就可能会出现部分读的情况，` read()` 将返回可用的字节数。

如果没有足够的缓冲区空间来传输所有请求的字节，并且满足下面的一条，就可能会出现部分写的情况：

- `write()` 调用传输了部分请求的字节后被信号处理例程中断
- 套接字工作在非阻塞模式下，可能当前只能传输一部分请求的字节
- 在部分请求的字节已经完成传输后出现了一个异步错误，比如，由于 TCP 连接出现问题，可能会使对端应用程序崩溃

如果出现了部分 IO 现象，那么有时候需要重新调用系统调用来完成数据的传输。

```
ssize_t readn(int fd,void* buffer,size_t count)
{
    ssize_t numRead;
    size_t totRead;
    char* buf;

    buf = buffer;
    for(totRead = 0;totRead < count;)
    {
        numRead = read(fd,buf,count- totRead);

        if(numRead == 0)
            return totRead;
        if(numRead == -1)
        {
            if(errno == EINTR)
                continue;
            else
                return -1;
        }

        totRead += numRead;
        buf += numRead;
    }

    return totRead;
}

ssize_t writen(int fd,void* buffer,size_t count)
{
    ssize_t numWritten;
    size_t totWritten;
    const char* buf;

    buf = buffer;
    for(totWritten = 0;totWritten < count;)
    {
        numWritten = write(fd,buf,count - totWritten);

        if(numWritten <= 0)
        {
            if(numWritten == -1 && errno == EINTR)
                continue;
            else
                return -1;
        }

        totWritten += numWritten;
        buf += numWritten;
    }

    return totWritten;
}
```

# `shutdown()` 系统调用

在套接字上调用 `close()` 会将双向通信通道的两端都关闭，系统调用 `shutdown()` 提供了只关闭一端的功能。

```
#include <sys/socket.h>

int shutdown(int sockfd, int how);
```

-  `how` 参数的选项：
  - `SHUT_RD`：
  - `SHUT_RD`：
  - `SHUT_RD`：

how的方式有三种分别是
SHUT_RD（0）：关闭sockfd上的读功能，此选项将不允许sockfd进行读操作。
SHUT_WR（1）：关闭sockfd的写功能，此选项将不允许sockfd进行写操作。
SHUT_RDWR（2）：关闭sockfd的读写功能。
————————————————
版权声明：本文为CSDN博主「cradle08」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/u011774239/article/details/34483281

























































