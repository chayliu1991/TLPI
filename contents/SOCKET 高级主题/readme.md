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

  - `SHUT_RD`：关闭连接的读端，之后的读操作将返回文件尾，数据仍然可以写入都套接字上，在 UNIX domain 流式 socket 上执行了此操作之后，对端应用程序将接收到一个 `SIGPIPE` 信号，如果继续尝试在对端套接字上做写操作将导致 `EPIPE` 错误
  - `SHUT_WR`：关闭连接的写端，一旦对端的应用程序已经将所有剩余的字节读取完毕，它就会检测到文件结尾，后续对本地套接字的写操作将产生 `SIGPIPE` 信号以及 `EPIPE` 错误，而由对端写入的数据仍然可以在套接字上读取
  - `SHUT_RDWR`：关闭连接的读端和写端

  需要注意的是：

  - 无论套接字是否还关联有其他的文件描述符，`shutdown()` 都会关闭套接字通道

  - `shutdown()` 不会关闭文件描述符，要关闭文件描述符，必须调用 `close()`

# 专用于套接字的 IO 系统调用: `recv()` 和 `send()`

```
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

-  `flags` 参数是一个位掩码，用来修改 IO 操作的行为，对于 `recv()` 来说，参数可以是下列的值：

  - `MSG_DONTWAIT`：让 `recv()` 以阻塞方式执行，如果没有数据可用，那么 `recv()` 不会阻塞而是立即返回，伴随错误码 `EAGAIN`，也可以通过 `fcntl() O_NONBLOCK` 从而达到相同效果
  - `MSG_OOB`：在套接字上接收带外数据
  - `MSG_PEEK`：从套接字缓冲区中获取一份请求字节的副本，但不会将请求的字节从缓冲区中实际移除，这份数据稍后可以由其他的  `recv()` 和 `read()` 调用重新读取
  - `MSG_WAITALL`：通常 `recv()` 调用返回的字节数要比请求的字节数少，指定了  `MSG_WAITALL` 标记后将导致系统调用阻塞，直到成功的接收到请求的字节数。但是，就算指定了这个标记，当出现下列情况时，该调用返回的字节数可能还是会少于请求的字节：
    - 捕获到一个信号
    - 流式套接字的对端终止了连接
    - 遇到了带外数据字节
    - 从数据报套接字接收到的消息长度小于 `len` 个字节
    - 套接字上出现了错误

- `send()` 可以指定的 `flags` 参数：

  - `MSG_DONTWAIT`：让 `send()` 以非阻塞方式执行，如果数据不能立刻传输(因为套接字发送缓冲区已满)，那么调用就不会阻塞，而是调用失败，伴随错误码 `EAGAIN`
  - `MSG_MORE`：在 TCP 套接字上，这个标记实现的效果同套接字选项 `TCP_CORK`  完成相同的功能，区别在于该标记可以在每次调用中对数据进行栓塞处理。这个标记也可以用于数据报套接字，在连续的 `send()` 或者 `sendto()` 调用中传输的数据，如果指定了该标志，那么数据会打包成一个单独的数据报，仅当下一次调用中没有指定该标记时数据才会传输出去。该标记对于 UNIX 域套接字没有任何效果 
  - `MSG_NOSIGNAL`：当在已连接的流式套接字上发送数据时，如果；连接的另一端已经关闭了，指定该标记后将不会产生 `SIGPIPE` 信号，相反，`send()` 调用会失败，伴随的错误码为 `EPIPE`
  
  - `MSG_OOB`：在流式套接字上发送带外数据
  
  # `sendfile()` 系统调用
  
  将磁盘上的文件内容不做修改地通过已连接套接字传输出去：
  
  ```
  while((n = read(diskfilefd,buf,BUF_SIZE)) > 0)
  	write(sockfd,buf,n);
  ```
  
  但是如果频繁的传输大文件的话，这种技术就显得不很高效，为了传输文件，必须使用两个系统调用：
  
  - 一个用来将文件内容从内核缓冲区 cache 中拷贝到用户空间
  - 另一个用来将用户空间缓冲区拷贝回内核空间中
  
  `sendfile()` 可以直接将文件内容传输到套接字上，而不会经过用户空间，这种技术成为零拷贝传输：

![](./img/send_file.png)

```
#include <sys/sendfile.h>

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);
```

- `out_fd` 必须指向一个套接字，`in_fd` 必须是可以进行 `mmap()` 操作的
- 可以使用 `sendfile()` 将数据从文件传递到套接字上，但是反过来不行
- 如果 `offset` 指定为 `NULL`  的话，那么从 `in_fd` 传输的字节就从当前的文件偏移量处开始，且在传输时会更新文件偏移量以反映出已传输的字节数
- `count` 指定了请求传输的字节数，如果在 `count` 个字节完成传输前就遇到了文件结尾符，那么只有文件结尾之前的那些字节能够被传输，调用之后，`sendfile()` 返回实际传输的字节数

## TCP_CORK 套接字选项

要进一步提高 TCP 应用使用 `sendfile()`  时的性能，采用 Linux 专有的套接字选项 `TCP_CORK` 常常会有帮助。

当在 TCP 套接字上启用了 `TCP_CORK` 选项之后，所有的输出都会缓冲到一个单独的 TCP 报文段中，直到满足以下条件为止：已达到报文段的大小上限，取消了 `TCP_CORK` 选项，套接字被关闭，或者当启用 `TCP_CORK` 之后，写入达到了超时时间。

可以通过 `setsockopt()` 系统调用来启用或者取消 `TCP_CORK` 选项。

# 获取套接字地址

```
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

- `getsockname()` 可以返回套接字地址族，以及套接字所绑定到的地址，当隐式绑定到一个 Internet  域套接字上事，如果想获取内核分配给套接字的临时端口号，那么调用 `getsockname()` 也是有用的，内核会在出现如下情况时执行一个隐式绑定：
  - 已经在 TCP 套接字上执行了 `connect()`  或者 `listen()` 调用，但是还没有通过 `bind()` 绑定一个地址上
  - 当在 UDP 套接字上首次调用 `sendto()`  时，该套接字之前还没有绑定到地址上
  - 调用 `bind()` 时将端口号指定为 0，这种情况下 `bind()` 会为套接字指定一个 IP 地址，但内核会选择一个临时端口号
-  `getpeername()` 返回流式套接字中对端套接字的地址，如果服务器想找出发出连接的客户端地址，这个调用就特别由于，主要用于 TCP 套接字上

# TCP 协议

## TCP 报文的格式

![](./img/tcp.png)

- 源端口号：TCP 发送端的端口号
- 目的源端口号：TCP 接收端的端口号
- 序列号：报文的序列号，标识从 TCP 发端向 TCP 收端发送的数据字节流，它表示在这个报文段中的第一个数据字节上
- 确认序号：如果设定了 ACK 位，那么这个字段包含了接收方期望从发送方接收到的下一个数据字节的序列号
- 首部长度：该字段用来表示 TCP 报文首部的长度，首部长度单位是 32 位，由于这个字段只占 4 个比特位，因此首部总长度最大可达到 60 字节，该字段使得 TCP 接收端可以确定变长的选项字段的长度，以及数据域的起始点
- 保留位：该字段包含 4 个未使用的比特位，目前应该设置为 0
- 控制位：包含 8 个比特位组成，能够进一步指定报文的含义：
  - `CWR`：拥塞窗口减小标记，congestion window reduced flag
  - `ECE`：显式的拥塞通知回显标记，explicit congestion notification echo flag
  - `URG`：如果设置了这个位，那么紧急指针字段包含的信息将是有效的
  - `ACK`：如果设置了该位，那么确认序号字段包含的信息是有效的
  - `PSH`：将所有收到的数据发送给接收的进程
  - `RST`：重置连接，用于处理多种错误情况
  - `SYN`：同步序列号，在连接连接时，双方需要交换设置了该位的报文，这样使得 TCP 连接的两端可以指定初始序号，在稍后可以双向传输数据
  - `FIN`：发送端提示完成了发送任务
- 窗口大小，该字段用在接收端发送 `ACK` 确认时提示自己可接受数据的空间大小
- 校验和：16 位的检验和包括 TCP 首部和 TCP 的数据域
- 紧急指针：如果设置了 `URG` 位，那么就表示从发送端到接收端传输的数据为紧急数据
- 选项：变长字段，包含了控制 TCP 连接操作的选项
- 数据：这个字段包含了该报文段中传输的用户数据，如果报文段没有包含任何数据，这个字段的长度为 0

## TCP 序列号和确认机制

![](./img/tcp_seq.png)

























































