# 迭代型和并发型服务器

对于使用 socket 的网络服务器程序，有两种常见的设计方式：

- 迭代型：服务器每次只处理一个客户端，只有当完全处理完一个客户端的请求后才去处理下一个客户端
- 并发型：能够同时处理多个客户端的请求

# 迭代型 UDP echo 服务器

`server`

```
int main(int argc,char* argv[])
{
    int sfd;
    ssize_t numRead;
    socklen_t addrLen,len;
    struct sockaddr_storage claddr;
    char buf[BUF_SIZE];
    char addrStr[IS_ADDR_STR_LEN];

    if(becomeDaemon(0) == -1)
        errExit("becomeDaemon()");
    
    sfd = inetBind(SERVICE,SOCK_DGRAM,&addrLen);
    if(sfd == -1)
    {
        syslog(LOG_ERR,"Could not create server socket (%s)",strerror(errno));
        exit(EXIT_FAILURE);
    }

    for(;;)
    {
        len = sizeof(struct sockaddr_storage);
        numRead = recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr*)&claddr,&len);
        if(numRead == -1)
            errExit("recvfrom()");
        
        if(sendto(sfd,buf,numRead,0,(struct sockaddr*)&claddr,len) != numRead)
        {
            syslog(LOG_WARNING,"Error echoing response to %s (%s)",inetAddressStr((struct sockaddr*)&claddr,len,addrStr,IS_ADDR_STR_LEN),strerror(errno));
        }
    }
}
```

`client`

```
int main(int argc,char* argv[])
{
    int sfd,j;
    size_t len;
    ssize_t numRead;
    char buf[BUF_SIZE];

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s host msg...\n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    sfd = inetConnect(argv[1],SERVICE,SOCK_DGRAM);
    if(sfd == -1)
        errExit("Colud not connect to server port");
    
    for(j = 2;j < argc;j++)
    {
        len = strlen(argv[j]);
        if(write(sfd,argv[j],len) != len)
            errExit("write()");
        
        numRead = read(sfd,buf,BUF_SIZE);
        if(numRead == -1)
            errExit("read()");
        
        printf("[%ld bytes] %.*s\n",(long)numRead,(int)numRead,buf);
    }
    
    exit(EXIT_SUCCESS);  
}
```

# 并发型 TCP echo 服务器

```
static void grimReaper(int sig)
{
    int savedErrno;

    savedErrno = errno;
    while(waitpid(-1,NULL,WNOHANG) > 0)
        continue;
    
    errno = savedErrno;
}

static void handleRequest(int cfd)
{
    char buf[BUF_SIZE];
    ssize_t numRead;

    while((numRead = read(cfd,buf,BUF_SIZE)) > 0)
    {
        if(write(cfd,buf,numRead))
        {
            syslog(LOG_ERR,"write() failed : %s",strerror(errno));
            exit(EXIT_SUCCESS);
        }
    }

    if(numRead == -1)
    {
        syslog(LOG_ERR,"Error from read() : %s",strerror(errno));
        exit(EXIT_SUCCESS);
    }
}

int main(int argc,char* argv[])
{
    int lfd,cfd;
    struct sigaction sa;

    if(becomeDaemon(0) == -1)
        errExit("becomeDaemon()");
    
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = grimReaper;
    if(sigaction(SIGCHLD,&sa,NULL) == -1)
    {
        syslog(LOG_ERR,"Error from sigaction() : %s",strerror(errno));
        exit(EXIT_FAILURE);
    }

    lfd = inetListen(SERVICE,10,NULL);
    if(lfd == -1)
    {
        syslog(LOG_ERR,"Could not create server socket : (%s)",strerror(errno));
        exit(EXIT_FAILURE);
    }

    for(;;)
    {
        cfd = accept(lfd,NULL,NULL);
        if(cfd == -1)
        {
            syslog(LOG_ERR,"Failure in accept : (%s)",strerror(errno));
            exit(EXIT_FAILURE);
        }
        
        switch(fork())
        {
            case -1:
                syslog(LOG_ERR,"Can not create child : (%s)",strerror(errno));
                close(cfd);
                break;
            case 0:
                close(lfd);
                handleRequest(cfd);
                _exit(EXIT_SUCCESS);
            default:
                close(cfd);
                break;
        }
    }
}
```

# 并发型服务器的其他设计方案

对于一个负载很高的服务器来说，为每个客户端创建一个新的子进程或者线程所带来的开销对服务器来说是沉重的负担。

可以考虑下面的几种方案：

## 在服务器上预先创建进程或线程

- 服务器程序在启动阶段(即在任何客户端请求到来之前)就立刻预先创建号一定数量的子进程(线程)，而不是针对每个客户端来创建一个新的子进程(线程)，这些子进程(线程)构成一个服务池
- 服务池中每个子进程一次只处理一耳光客户端，在处理完客户端请求后，子进程并不会终止，而是获取下一个待处理的客户端继续处理

采用上述的服务池时，在负载高峰期应该动态增加服务池的大小，在负载降低时，应该相应地降低服务池大小。

## 在单个进程中处理多个客户端

为了实现这一点，必须采用一种允许单个进程同时监视多个文件描述符 IO 事件的 IO 模型。

必须依靠内核来确保每个服务进程能公平地访问到服务器主机的资源。

## 采用服务器集群

用来处理高客户端负载的方法还包括使用多个服务器系统，即服务器集群。

构建服务器集群最简单的方法就是 DNS 轮转负载共享(DNS round-robin load sharing)或者负载分发(load distribution)。一个地区的域名权威服务器将同一个域名映射到多个 IP 地址上，后续对 DNS 服务器的域名解析请求将以循环轮转的方式以不同的顺序返回这些 IP 地址。

DNS 循环轮转的优势是成本低，而且容易实施。但是也存在一些问题，其中一个问题是远端 DNS 服务器上所执行的缓存操作，这意味着今后位于某个特定主机上的客户端发出的请求会绕过循环轮转 DNS 服务器，并总是由同一个服务器来负责处理。此外，循环轮转 DNS 并没有任何内建的用来确保到达良好负载均衡或者是确保高可用性的机制。

# inetd(Internet 超级服务器)守护进程

守护进程 inetd 被设计用来消除运行大量非常用服务器进程的需要，inetd 可提供两个主要的好处：

- 与其为每个服务运行一个单独的守护进程，现在只用一个进程 inetd 守护进程，就可以监视一组指定的套接字端口，并按照需要启动其他的服务，从而可以降低系统上运行的进程数量
- inetd 简化了启动其他服务的编程工作，因为由  inetd 执行的一些步骤通常在所有的网络服务启动时都会用到

## inetd 守护进程所做的操作

inetd 守护进程通常在系统启动时运行，在成为守护进程后，inetd 执行的步骤：

- 对于在配置文件 `/etc/inetd.conf` 中指定的每个服务，inetd 都会创建一个恰当类型的套接字，然后绑定到指定的端口上，每个 TCP 都会通过 `listen()` 调用允许客户端来连接
- 通过 `select()` 调用，inetd 对前一步中创建的所有套接字进行监视，看是否有数据报或请求连接发送过来
- `select()`  调用进入阻塞，直到一个 UDP 套接字上有数据报可读或者 TCP 套接字上收到了连接请求，在 TCP 连接中，inetd 在进入下一个步骤之前会先为连接执行 `accept()`
- 要启动这个套接字上指定的服务，inetd 调用 `fork()` 创建一个新的进程，然后通过 `exec()` 启动服务器程序，在执行 `exec()` 之前，子进程执行如下步骤：
  - 除了用于 UDP 数据报和接受  TCP 连接的文件描述符外，将其他所有从父进程继承而来的文件描述符都关闭
  - 在文件描述符 0,1,2 上复制套接字文件描述符，并关闭套接字文件描述符本身
  - 这一步是可选的，为启动的服务器进程设定用户和组 ID，设定的值可以在 `/etc/inetd.conf`  中相应条目找到
- 在 TCP 连接上接受一个连接，inetd 就关闭这个套接字
- 跳回到 `select()` 步骤继续执行

## `/etc/inetd.conf`  文件

 `/etc/inetd.conf` 文件中的每一行都描述一种由  inetd 处理的服务，包含以下字段：

- 服务名称
- 套接字类型
- 协议
- 标记，该字段的内容要么是 `wait`，要么是 `nowait`。表明了由 inetd 启动的服务器是否会接管用于该服务的套接字，如果启动的服务器需要管理这个套接字，那么就指定为 `wait`
- 登录名
- 服务器程序
- 服务器程序参数

当修改了 `/etc/inetd.conf`  文件之后，需要发送一个 `SIGHUP` 信号给 inetd，请求其重新读取配置文件：

```
kill -HUP inted
```









































