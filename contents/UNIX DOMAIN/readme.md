# UNIX domain socket 地址

UNIX domain 中，socket 地址以路径名来表示，domain 特定的 socket 地址结构定义如下：

```
struct sockaddr_un {
    sa_family_t sun_family; 		/* PF_UNIX 或 AF_UNIX */
    char sun_path[UNIX_PATH_MAX];	 /* 路径名 */
};
```

为将一个 UNIX domain socket 绑定到一个地址上，需要初始化一个 `sockaddr_un` 结构，然后将指向这个结构的一个指针作为 `addr` 参数传入 `bind()` 并将 `addrlen` 指定为这个结构的大小：

```
const char* SOCKNAME = "/tmp/mysock";
int sfd;
struct sockaddr_un addr;

sfd = socket(AF_UNIX,SOCK_STREAM,0);
if(sfd == -1)
	errExit("socket()");

memset(&addr,0,sizeof(struct sockaddr_un));
addr.sun_family = AF_UNIX;
strncpy(addr.sun_path,SOCKNAME,sizeof(addr.sun_path) - 1);

if(bind(sfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un)) == -1)
	errExit("bind()");
```

当用来绑定 UNIX domain socket 时，`bind()` 会在文件系统中创建一个条目。这个文件会被标记为一个 socket，当在这个路径名上应用 `stat()` 时，它会在 `stat`  结构的 `st_mode` 字段中的文件类型部分返回值 `S_IFSOCK`。当使用 `ls -l` 列出时，UNIX domain socket 在第一列将会显示 `s`，而 `ls -F` 则会在 socket 路径名后面加上一个等号 `=`。

尽管 UNIX domain socket 是通过路径名来标识的，但在这些 socket 上发生的 IO 无须对底层设备进行操作。

绑定一个 UNIX domain socket 方面还需要注意以下几点：

- 无法将一个 socket 绑定到一个既有路径名上，`bind()` 会失败并返回 `EADDRINUSE`  错误
- 通常会将一个 socket 绑定到一个绝对路径名上，这样这个 socket 就会位于文件系统中的一个固定地址，也可以使用一个相对路径名，但是并不常见，因为它要求想要 `connect()` 这个 socket 的应用程序知道执行 `bind()` 的应用程序的当前工作目录
- 一个 socket 只能绑定到一个路径名上，相应地，一个路径名只能被一个 socket 绑定
- 无法使用 `open()` 打开一个 socket
- 当不再需要一个 socket 时可以使用 `unlink()` 或者 `remove()` 删除其路径名条目

# UNIX domain 中的流 socket

`server`：

```
int main(int argc,char* argv[])
{
    struct sockaddr_un addr;
    int sfd,cfd;
    ssize_t numRead;
    char buf[BUFSIZ];


    sfd = socket(AF_UNIX,SOCK_STREAM,0);	//@ 创建一个 socket
    if(sfd == -1)
        errExit("socket()");
    
    if(remove(SV_SOCK_PATH) == -1 && errno != ENOENT)  //@ 移除所有的既有文件
        errExit("remove()");
    
    //@ 创建地址结构
    memset(&addr,0,sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,SV_SOCK_PATH,sizeof(addr.sun_path)-1);
    if(bind(sfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un)) == -1)
        errExit("bind()");

    if(listen(sfd,BACKLOG) == -1)
        errExit("listen()");

	//@ 执行一个无限循环
    for(;;)
    {   
        cfd = accept(sfd,NULL,NULL);  //@ 接受一个连接
        if(cfd == -1)
            errExit("accept()");
        
        while((numRead = read(cfd,buf,BUFSIZ)) > 0) //@ 读取数据
        {
            if(write(STDOUT_FILENO,buf,numRead) != numRead) //@  写入数据
                errExit("write()");
        }

        if(numRead == -1)
             errExit("read()");

        if(close(cfd) == -1)	//@ 关闭连接
             errExit("close()");
    }
}
```

`client`

```
int main(int argc,char* argv[])
{
    struct sockaddr_un addr;
    int sfd;
    ssize_t numRead;
    char buf[BUFSIZ];

    sfd = socket(AF_UNIX,SOCK_STREAM,0); //@ 创建一个 socket
    if(sfd == -1)
        errExit("socket()");
    
    //@ 创建地址结构
    memset(&addr,0,sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,SV_SOCK_PATH,sizeof(addr.sun_path) - 1);

	//@ 执行连接
    if(connect(sfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un)) == -1)
        errExit("connect()");
    
    while((numRead = read(STDIN_FILENO,buf,BUFSIZ)) > 0)
    {
        if(write(sfd,buf,numRead) != numRead)
            errExit("write()");
    }

    if(numRead == -1)
        errExit("read()");
    
    exit(EXIT_SUCCESS);
}
```

# UNIX domain 中的流数据报 socket

对于 UNIX domain socket 来讲，数据报的传输是在内核中发生的，并且也是可靠的。所有的消息都会按序传递并且不会发生重复。

## UNIX domain 数据报 socket 能传输的数据报的最大大小

在 Linux 上数据报的限制是通过 `SO_SNDBUF` socket 选项和各个 `/proc` 文件来控制的。

但其他一些 UNIX 实现采用的限制值更小一些，如 2048 字节。

`server`：

```
int main(int argc,char* argv[])
{
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    ssize_t numBytes;
    socklen_t len;
    char buf[BUF_SIZE];

    sfd = socket(AF_UNIX,SOCK_DGRAM,0);
    if(sfd == -1)
        errExit("socket()");
    
    if(remove(SV_SOCKET_PATH) == -1 && errno != ENOENT)
        errExit("remove()");

    memset(&svaddr,0,sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path,SV_SOCKET_PATH,sizeof(svaddr.sun_path)-1);

    if(bind(sfd,(struct sockaddr*)&svaddr,sizeof(struct sockaddr_un)) == -1)
        errExit("bind()");

    for (;;)
    {
        len = sizeof(struct sockaddr_un);
        numBytes = recvfrom(sfd,buf,BUF_SIZE,0,(struct sockaddr*)&claddr,&len);
        if(numBytes == -1)
            errExit("recvfrom()");
        printf("server received %ld bytes from %s\n",(long)numBytes,claddr.sun_path);

        for (j = 0; j < numBytes;j++)
            buf[j] = toupper((unsigned char)buf[j]);
        
        if(sendto(sfd,buf,numBytes,0,(struct sockaddr*)&claddr,len) != numBytes)
            errExit("sendto()");
    }
}

```

`client`

```
int main(int argc,char* argv[])
{
    struct sockaddr_un svaddr, claddr;
    int sfd, j;
    size_t msgLen;
    ssize_t numBytes;
    char resp[BUF_SIZE];

    if(argc < 2 || strcmp(argv[1],"--help") == 0)
    {
        printf("%s msg ... \n",argv[0]);
        exit(EXIT_SUCCESS);
    }

    sfd = socket(AF_UNIX,SOCK_DGRAM,0);
    if(sfd == -1)
        errExit("socket()");
    
    memset(&claddr,0,sizeof(struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf(claddr.sun_path,sizeof(claddr.sun_path),"/tmp/ud_ucase_cl.%ld",(long)getpid());

    if(bind(sfd,(struct sockaddr*)&claddr,sizeof(struct sockaddr_un)) == -1)
        errExit("bind()");

    memset(&svaddr,0,sizeof(struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy(svaddr.sun_path,SV_SOCKET_PATH,sizeof(svaddr.sun_path)-1);

    for (j = 1; j < argc;j++)
    {
        msgLen = strlen(argv[j]);
        if(sendto(sfd,argv[j],msgLen,0,(struct sockaddr*)&svaddr,sizeof(struct sockaddr_un)) != msgLen)
            errExit("sendto()");

        numBytes = recvfrom(sfd,resp,BUF_SIZE,0,NULL,NULL);
        if(numBytes == -1)
            errExit("recvfrom()");

        printf("Response %d: %.*s\n",j,(int)numBytes,resp);
    }

    remove(claddr.sun_path);
    exit(EXIT_SUCCESS);
}
```

# UNIX domain socket 权限

socket 文件的所有权和权限决定了哪些进程能够与这个 socket 进行通信：

- 要连接一个 UNIX domain 流 socket 需要在该 socket 文件上拥有写权限
- 要通过一个 UNIX domain 流数据报 socket 发送一个数据报需要在该 socket 文件上拥有写权限

此外，需要在存放 socket 路径名的所有目录上又有执行权限。

在默认情况下，创建 socket 时会给所有者(用户)、组以及 other 用户 赋予所有的权限，要改变这种行为可以在调用 `bind()` 之前先调用 `umask()` 来禁用不希望赋予的权限。

# 创建互联 socket 对

有时候让当个进程创建一对 socket 并将它们连接起来是比较有用的。这可以通过使用两个 `socket()` 调用和一个 `bind()` 调用以及对 `listen()`、`connect()`、`accept()` 的调用或对 `connect()` 的调用来完成，`socketpair()` 则为这个操作提供了一个快捷方式：

```
#include <sys/types.h>
#include <sys/socket.h>

int socketpair(int domain, int type, int protocol, int sv[2]);
```

- `socketpair()` 只能用在 UNIX domain 中，即 `type` 必须是 `AF_UNIX`
- `type` 可以被指定为 `SOCK_DGRAM` 或者 `SOCK_STREAM`
- `protocol` 必须是 0
- `sv` 返回引用了这两个相互连接的 socket 的文件描述符

一般来说，socket 对的使用方式与管道的使用方式类似，在调用完 `socketpair()` 之后，进程会使用 `fork()` 创建一个子进程，子进程会继承父进程的文件描述符的副本，因此父子进程可以使用一对 socket 进行 IPC。

使用 `socketpair()` 创建一对 socket  与手工创建一对相互连接的 socket 这两种做法之间的一个差别在于前一对 socket 不会绑定到任意地址上，因为这对 socket 对其他进程是不可见的。

# Linux 抽象 socket 名空间

所谓的抽象路径名空间是 Linux 特有的，它允许将一个 UNIX domain socket 绑定到一个名字上但不会在文件系统中创建该名字，其具备的优势：

- 无需担心与文件系统中既有名字产生冲突
- 没有必要在使用完 socket 之后删除 socket 路径名，当 socket 被关闭后会自动删除这个抽象名
- 无需为 socket 创建一个文件系统路径名，这对于 chroot 环境以及在不具备文件系统写权限时是比较有用的

要创建一个抽象绑定就需要将 `sun_path` 字段的第一个字节指定为 `NULL` 即 `\0`。

```
struct sockaddr_un addr;

memset(&addr,0,sizeof(struct sockaddr_un));
addr.sun_family = AF_UNIX;

strncpy(&addr.sun_path[1],"xyz",sizeof(addr.sun_path)-2);

sockfd = socket(AF_UNIX,SOCK_STREAM,0);
if(sockfd == -1)
	errExit("socket()");
	
if(bind(sockfd,(struct sockaddr*)&addr,sizeof(struct sockaddr_un)) == -1)
	errExit("bind()");
```















































