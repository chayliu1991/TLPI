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



# 创建互联 socket 对





# Linux 抽象 socket 名空间















































