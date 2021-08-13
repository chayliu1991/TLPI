# Internet domain socket

Internet domain  流 socket 是基于 TCP 的，它们提供了可靠的双向字节流通信信道。

Internet domain 数据报 socket 是基于 UDP 的：

- UNIX domain 数据报 socket 是可靠的，但是 UDP socket 则不是可靠的，数据报可能会丢失，重复，乱序
- 在一个 UNIX domain 数据报 socket 上发送数据会在接收 socket 的数据队列为满时阻塞，与之不同的是，使用 UDP 时如果进入的数据报会使接收者的队列溢出，那么数据报就会静默地被丢弃

# 网络字节序

2 字节和 4 字节整数的大端和小端字节序：

![](./img/byte_order.png)

网络字节序采用大端。

`INADDR_ANY` 和 `INADDR_LOOPBACK` 是主机字节序，在将它们存储进 socket 地址结构中之前需要将这些值转换成网络字节序。

主机序和网络字节序之间的转换：

```
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);
uint16_t htons(uint16_t hostshort);
uint32_t ntohl(uint32_t netlong);
uint16_t ntohs(uint16_t netshort);
```

# 数据表示

`readLine()` 从文件描述符 `fd` 引用的文件中读取字节直到碰到换行符为止。

```
ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;
    size_t toRead;
    char *buf;
    char ch;


    if(n <= 0 || buffer == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;
    toRead = 0;
    for (;;)
    {
        numRead = read(fd,&ch,1);
        if(numRead == -1)
        {
            if(errno == EINTR)
                continue;
            else
                return -1;
        }
        else if(numRead == 0)
        {
            if(toRead == 0)
                return 0;
            else
                break;
        }  
        else
        {
            if(toRead < n-1)
            {
                toRead++;
                *buf++ = ch;
            }
            if(ch == '\n')
                break;
        }       
    }

    *buf = '\0';
    return toRead;
}
```

# Internet socket 地址

Internet  domain socket 地址有两种：IPv4 和 IPv6。

## IPv4 socket 地址

IPv4 地址存储于结构体 `sockaddr_in` 中：

```
struct in_addr {
	uint32_t       s_addr;     /* address in network byte order */
};

struct sockaddr_in{
	sa_family_t sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	unsigned char __pad[X];
};
```

- `sin_family` 总是 `AF_INET`
- `in_port_t` 和 `in_addr` 分别是端口号和 IP 地址，它们都是网络字节序，分别是 16 位和 32 位

## IPv6 socket 地址

```
struct in6_addr{
	uint8_t s6_addr[16];
};

struct sockaddr_in6{
	sa_family_t sin6_family;
	in_port_t sin6_port;
	uint32_t  sin6_flowinfo;
	struct in6_addr sin6_addr;
	uint32_t  sin6_scope_id;
};
```

IPv6 的通配地址 `0::0`，换回地址为 `::1`。

## sockaddr_storage 结构

IPv6 socket API 中新引入了一个通用的 `sockaddr_storage ` 结构，这个结构的空间足以容纳任意类型的 socket：

```
#define __ss_aligntype uint32_t
struct sockaddr_storage{
	sa_family ss_family;
	__ss_aligntype __ss_slign;
	char __ss_padding[SS_PADSIZE];
};
```

# 主机和服务转换函数概述





























































