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





# UNIX domain 中的流数据报socket



# UNIX domain socket 权限



# 创建互联 socket 对





# Linux 抽象 socket 名空间















































