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

