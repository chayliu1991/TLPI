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

主机名和连接在网络上的一个系统的符号标识符，服务名是端口号的符号表示。主机地址和端口的表示有下列两种：

主机地址和端口的表示有两种方法：

- 主机地址可以表示为一个二进制值或一个符号主机名或展现格式(IPv4 点分十进制，IPv6 是十六进制字符串)
- 端口号可以表示为一个二进制值或一个符号服务名

# `inet_pton()` 和 `inet_ntop()` 函数

```
#include <arpa/inet.h>

int inet_pton(int af, const char *src, void *dst);
const char *inet_ntop(int af, const void *src,char *dst, socklen_t size);
```

- `p` 表示展现 `presentation` ，`n` 表示 网络 `network`
- `inet_pton()` 将 `src` 包含的展现字符串转换成网络字节序的二进制 IP 地址，`af`  被指定为 `AF_INET`  或者  `AF_INET6`
-  `inet_ntop()` 执行逆向转换， `size`  被指定为缓冲器的大小，如果 `size` 太小，那么 `inet_ntop()` 会返回 `NULL` 并将 `errno` 设置成 `ENOSPC`

缓冲器大小可以使用下面两个宏指定：

```
#include <netinet/in.h>

#define INET_ADDRSTRLEN		 16
#define INET6_ADDRSTRLEN	 46
```

# 数据报 socket 客户端/服务器示例

`server`

```
int main(int argc,char* argv[])
{
    struct sockaddr_in6 svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];
    char claddrStr[INET_ADDRSTRLEN];

    sfd = socket(AF_INET6,SOCK_DGRAM,0);
    if(sfd ==-1)
        errExit("socket()");

    memset(&svaddr,0,sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6;
    svaddr.sin6_addr = in6addr_any;
    svaddr.sin6_port = htons(PROT_NUM);

    if(bind(sfd,(struct sockaddr_in6*)&svaddr,sizeof(struct sockaddr_in6)) == -1)
        errExit("bind()");

    for (;;)
    {
        len = sizeof(struct sockaddr_in6);
        numBytes = recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr*)&claddr,&len);
        if(numBytes == -1)
            errExit("recvfrom()");
        if (inet_ntop(AF_INET6, &claddr.sin6_addr, claddrStr,INET6_ADDRSTRLEN) == NULL)
            printf("could not convert client address to string\n");
        else
            printf("Sever received %ld bytes from (%s,%u)\n",(long)numBytes,claddr,ntohs(claddr.sin6_port));
        
        if(sendto(sfd,buf,numBytes,0,(struct sockaddr*)&claddr,len) != numBytes)
            errExit("sendto()");
    }
}
```

`client`

```
int main(int argc,char* argv[])
{
    struct sockaddr_in6 svaddr, claddr;
    int sfd, j;
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];

    if(argc < 3 | strcmp(argv[1],"--help") == 0)
    {
        printf("%s host-address msg...",argv[0]);
        exit(EXIT_SUCCESS);
    }

    sfd = socket(AF_INET6,SOCK_DGRAM,0);
    if(sfd ==-1)
        errExit("socket()");

    memset(&svaddr,0,sizeof(struct sockaddr_in6));
    svaddr.sin6_family = AF_INET6;
    svaddr.sin6_port = htons(PROT_NUM);
    if(inet_pton(AF_INET6,argv[1],&svaddr.sin6_addr) <= 0)
        errExit("inet_pton()");

    for (j = 2; j < argc;j++)
    {
        msgLen = strlen(argv[j]);
        if (sendto(sfd,argv[j],msgLen,0,(struct sockaddr*)&svaddr,sizeof(struct sockaddr_in6)) != msgLen)
            errExit("sendto()");

        numBytes = recvfrom(sfd,resp,BUF_SIZE,0,NULL,NULL);
        if(numBytes == -1)
            errExit("recvfrom()");

        printf("Respone %d : %.*s\n",j-1,(int)numBytes,resp);
    }

    exit(EXIT_SUCCESS);
}
```

# 域名系统(DNS)

DNS 出现以前，主机名和 IP 地址之间的映射关系是在一个手工维护的本地文件 `/etc/hosts`  中进行定义的：

```
127.0.0.1       localhost
::1     ip6-localhost ip6-loopback
```

`gethostbyname()` 或者 `getaddrinfo()` 通过搜索这个文件并找出与规范主机名或其中一个别名匹配的记录来获取一个 IP 地址。

DNS 设计：

![](./img/dns.png)

- 将主机名组织在一个层级名空间中，每个节点有一个标签，该标签最多能包含 63 个字符，层级的根是一个无名的节点，称为 "匿名节点"
- 一个节点的域名由该节点到根节点的路径中所有节点的名字连接而成，各个名字之间使用 `.` 分隔
- 完全限定域名，如 `www.kernel.org.` ，标识出了层级中的一台主机，区分一个完全限定域名的方法是看名字是否以`.`结尾，但是在很多情况下这个点会被省略
- 没有一个组织或系统会管理整个层级，相反，存在一个 DNS 服务器层级，每台服务器管理树的一个分支(区域)
- 当一个程序调用 `getaddrinfo()` 来解析一个域名时，`getaddrinfo()` 会使用一组库函数来与各地的 DNS 服务器通信，如果这个服务器无法提供所需要的信息，那么它就会与位于层级中的其他  DNS 服务器进行通信以便获取信息，这个过程可能花费很多时间，DNS 采用了缓存技术以节省时间

## 递归和迭代的解析请求

DNS 解析请求可以分为：递归和迭代。

递归请求：请求者要求服务器处理整个解析任务，包括在必要时候与其它 DNS 服务器进行通信任务。当位于本地主机上的一个应用程序调用 `getaddrinfo()`  时，该函数会与本地 DNS 服务器发起一个递归请求，如果本地 DNS 服务器自己没有相关信息来完成解析，那么它就会迭代地解析这个域名。

迭代解析：假设要解析 `www.otago.ac.nz`，首先与每个 DNS 服务器都知道的一小组根名字服务器中的一个进行通信，根名字服务器会告诉本 DNS 服务器到其中一台 `nz` DNS 服务器上查询，然后本地 DNS 服务器会在 `nz` 服务器上查询名字 `www.otago.ac.nz`，并收到一个到 `ac.nz` 服务器上查询的响应，之后本地 DNS 服务器会在 `ac.nz` 服务器上查询名字 `www.otago.ac.nz` 并告知查询 `otago.ac.nz` 服务器，最后本地 DNS 服务器会在 `otago.ac.nz` 服务器上查询 `www.otago.ac.nz` 并获取所需的 IP 地址。

向  `gethostbyname()` 传递一个不完整的域名，那么解析器在解析之前会尝试补齐。域名补全规则在  `/etc/resolv.conf` 中定义，默认情况下，至少会使用本机的域名来补全，例如，登录机器 `oghma.otago.ac.nz` 并输入 `ssh octavo` 得到的 DNS 查询将会以 `octavo.otago.ac.nz` 作为其名字。

































































