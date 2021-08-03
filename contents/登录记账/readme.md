# utmp 和 wtmp 文件概述

UNIX 系统维护着两个包含与用户登录和登出有关的信息的数据文件：

- utmp 文件维护着当前登录进系统的用户记录，每一个用户登录进系统时都会向 utmp 文件写入一条记录，当用户登出时该条记录会被删除，`who` 命令会使用 utmp 文件的信息
- wtmp 文件包含着所有用户登录和登出行为的留痕信息以供审计之用，每一个用户登录进系统时，写入 utmp 文件的记录同时会被附加到 wtmp 文件中，当用户登出系统的时候还会向这个文件附加一条记录，`last`  命令会使用 wtmp 文件的信息

在 Linux 下，utmp 文件位于 `/var/run/utmp`，wtmp 文件位于 `/var/log/wtmp`。但是一般不需要使用这些路径，而是使用 `<paths.h>` 和 `<utmpx.h>` 中定义的 `_PATH_UTMP` 和 `_PATH_WTM`。

# utmpx 结构

utmp 和 wtmp 文件包含 utmpx 记录。定义在 `<utmpx.h>` 中：

```
#define _GUN_SOURCE

struct exit_status {
    short e_termination;   /* Process terminate status */
    short e_exit;          /* Process exit status */
};

#define __UT_NAMESIZE   32
#define __UT_HOSTSIZE  256

struct utmpx {
    short   ut_type;                   /* Type of record */
    pid_t   ut_pid;                    /* PID of login process */
    char    ut_line[__UT_LINESIZE];    /* Terminate device name */
    char    ut_id[4];                  /* Suffix from terminate name, or ID field from inittab */
    char    ut_user[__UT_LINESIZE];    /* Username */
    char    ut_host[__UT_HOSTSIZE];    /* Hostname for remote login, or kernel version for run-level message */
    struct  exit_status ut_exit;       /* Exit status of process marked as DEAD_PROCESS */
    long    ut_session;                /* Session ID */
    struct timeval ut_tv;              /* Time when entry was made */
    int32_t ut_addr_v6[4];             /* IP address of remote host (IPv4 address uses just ut_addr_v6p[0] with other elements set to 0) */
    char    __unused[20];
};
```

- `utmpx` 结构中的所有字符串字段都以 `null` 结尾，除非值完全填满了相应数组

- 对于登录进程来讲，存储在 `ut_line` 和 `ut_id` 字段中的信息是从终端设备的名称中得出的，`ut_line` 字段包含了终端设备的完整的文件名， `ut_id` 包含了文件名的后缀，例如对于 `/dev/tty2` 终端，`ut_line` 是 `tty2`，`ut_id` 的值是 2

- 在窗口环境中，一些终端模拟器使用 `ut_session` 字段来为终端窗口记录会话 ID

- `ut_type` 字段是一个整数，它定义了写入文件的记录类型，其取值为下面一组常量中的值：

  - `EMPTY(0)`：这个记录不包含有效的记账信息
  - `RUN_LVL(1)`：这个记录表明在系统启动或关闭系统运行级别发生了变化，要在 `<utmpx.h>`中取得这个常量，需要定义 `_GNU_SOURCE` 特性测试宏
  - `BOOT_TIME(2)`：这个记录包含 `ut_tv` 字段中的系统启动时间，写入 `RUN_LVL` 和 `BOOT_TIME` 字段的进程通常是 init 进程，这些记录会同时被写入 `utmp` 和 `wtmp` 文件
  - `NEW_TIME(3)`：这个记录包含系统时钟变更之后的新时间，记录在 `ut_tv` 字段中
  - `OLD_TIME(4)`：这个记录包含系统时钟变更之前的旧时间，记录在 `ut_tv`  字段中，当系统时钟发生变更时，NTP daemon 会将类型为 `LOD_TIME`  和 `NEW_TIME` 的记录写入到 `utmp` 和 `wtmp` 文件中
  - `INIT_PROCESS(5)`：记录由 init 进程孵化的进程，如 `getty` 进程
  - `LOGIN_PROCESS(6)`：记录用户登录会话组长进程，如 `login` 进程
  - `USER_PROCESS(7)`：记录用户进程，通常是登录会话，用户名会出现在 `ut_user` 字段中，登录会话可能是由 `login` 启动，也可能是由像 ftp 和 ssh 之类的远程登录工具的应用程序启动
  - `DEAD_PROCESS(8)`：这个记录标识出已经退出的进程


# 从 utmp 和 wtmp 文件中检索信息

```
#include <utmpx.h>

void setutxent(void);
```

- `setutxent` 函数会将 utmp 文件的当前位置设置到文件的起始位置

- 通常，在使用任意 `getutx*` 函数之前应该调用 `setutxent()`，这样就能避免因程序中已经调用到的第三方函数在之前用过这些函数而产生混淆
- 当 utmp 文件没有被打开时，`setutxent()` 函数和 `getutx*` 函数会打开这个文件。当用完这个文件之后可以使用 `endutxent()` 函数来关闭这个文件

```
#include <utmpx.h>

void endutxent(void);
```

`getutxent()`、`getutxid()` 和 `getutxline()` 函数从 utmp 文件中读取一个记录并返回一个指向 utmpx 结构（静态分配）的指针：

```
#include <utmpx.h>

struct utmpx *getutxent(void);
struct utmpx *getutxid(const struct utmpx *ut);
struct utmpx *getutxline(const struct utmpx *ut);
```

- `getutxent()` 函数顺序读取下一个 utmp 文件中的下一个记录，`getutxid()` 和 `getutxline()` 函数会从当前文件位置开始搜索与 `ut` 参数指向的 utmpx 结构中指定的标准匹配的一个记录
- `getutxid()` 函数根据 `ut` 参数中 `ut_type` 和 `ut_id` 字段的值在 utmp 文件中搜索一个记录

- 如果 `ut_type` 字段是 `RUN_LVL`、`BOOT_TIME`、`NEW_TIME` 或 `OLD_TIME`，那么 `getutxid()` 会找出下一个 `ut_type` 字段与指定的值相匹配的记录(这种类型的记录与用户登录不相关）。这样就能够搜索与修改系统时间和运行级别相关的记录了
- 如果 `ut_type` 字段是剩余有效值中的一个，那么 `getutxid()` 会找出下一个 `ut_type` 字段与这些值中的任意一个匹配且 `ut_id` 字段与 `ut` 参数相同的记录，这样就能够扫描文件来找出对应于某个特定终端的记录了

`getutxline()` 函数会向前搜索 `ut_type` 字段为 `LOGIN_PROCESS` 或 `USER_PROCESS` 并且 `ut_line` 字段与 `ut` 参数指定的值匹配的记录。这对于找出与用户登录相关的记录是非常有用的。

当搜索失败时（即达到文件结尾时还未找到匹配的记录），`getutxid()` 和 `getutxline()` 都返回 `NULL`。

由于 `getutx*` 函数返回的是一个指向静态分配的结构的指针，因此它们是不可重入的。GNU C 库提供了传统的 utmp 函数的可重入版本（`getutent_r()`、`getutid_t()`、`getutline_r()`），但并没有为 utmpx 函数提供可重入版本。

在默认情况下， 所有 `getutx*` 函数都使用标准的 utmp 文件。如果需要使用另一个文件，如 wtmp 文件，那么必须要首先调用 `utmpxname()` 并指定目标路径名。

```
#define _GUN_SOURCE
#include <utmpx.h>

int utmpxname(const char *file);
```

`utmpxname()` 函数仅仅将传入的路径名复制一份，它不会打开文件，但会关闭之前由其他调用打开的所有文件。这就表示就算制定了一个无效的路径名，`utmpxname()` 也不会返回错误。

# 获取登录名称：`getlogin()`

`getlogin()` 函数返回登录到调用进程的控制终端的用户名，它会使用在utmp文件中维护的信息：

```
#include <unistd.h>

char *getlogin(void);
int getlogin_r(char *buf, size_t bufsize);
```

