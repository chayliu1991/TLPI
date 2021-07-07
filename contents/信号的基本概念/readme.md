# 概念和概述

信号是事件发生时对进程的通知机制，有时称为软件中断，信号与硬件中断类似之处是：打断了程序执行的正常流程，大多数情况下，无法预测信号到达的准确时间。

一个具有合适权限的进程可以向另一个进程发送信号，进程也可以给自身发送信号，但多数信号源于内核，内核为进程产生信号的各类事件：

- 硬件异常，例如：被除0，引用了无法访问的内存区域
- 用户键入能够产生信号的终端字符，例如：Control+c，Control+z
- 发生软件事件，例如：定时器到期，进程执行的 CPU 时间超限，子进程退出

针对每个信号都定义了唯一一个小整数，从1开始顺序展开，`<signal.h>`  以 `SIGXXXXX`  形式的符号命名。

信号分为两大类：

- 第一组用于内核向进程通知事件，构成所谓传统或者标准信号，Linux 中标准信号的范围 1~31
- 另一组由实时信号构成

信号由于某些事件产生，产生信号后，会于稍后被传递给某一进程，而进程也会采取某些措施来响应信号，在产生和到达之间，信号处于等待状态。

通常，一旦内核接下来要调度该进程运行，等待信号将马上送达，或者如果进程正在运行，则会立即传递信号。如果一段代码不想被信号打断，则可以将该信号添加到进程信号掩码中，从而阻塞信号到达，直到信号从掩码中移除，才会送达。

信号到达后，进程的默认行为：

- 忽略信号，内核将丢弃信号，该信号对进程没有任何影响
- 终止进程，有时指的是进程异常终止，而不是进程调用 `exit()` 发生的正常终止
- 产生核心转储文件，同时终止进程，核心转储文件包含对进程虚拟内存的镜像，可将其加载到调试器检查进程终止时的状态
- 停止进程，暂停进程的执行
- 恢复之前停止的进程

除了默认行为，程序也可以改变信号到达时的响应行为：

- 采取默认行为
- 忽略信号
- 执行信号处理器函数

注意，无法将信号处理设置为终止进程或者转储核心，除非这是信号的默认处置，效果最为近似的是为信号安装一个处理器程序，并于其中调用 `exit()` 或者 `abort()` ，`abort()` 为进程产生一个 `SIGABRT` 信号，该信号将引发转储核心文件并终止进程。

# 信号类型和默认行为

- `SIGABRT` ：进程调用 `abort()` ，系统向进程发送该信号，默认情况下，该信号会终止进程，并产生核心转储文件
- `SIGALARM` ：经过 `alarm()`  或者 `setitimer()` 设置的定时器到期时，内核将产生该信号
- `SIGBUS` ：产生该信号表示发生了某种内存访问错误
- `SIGCHILD`：父进程的某一子进程终止时，内核向父进程发送该信号，父进程的某一子进程收到信号而停止或者恢复时，也可能向父进程发送该信号
- `SIGCLD` ：与 `SIGCHILD` 信号同义
-  `SIGCONT`：将该信号发送给已停止的进程，进程将恢复运行，当接收信号的进程当前不处于停止状态时，默认情况下将忽略该信号
- `SIGEMT`：UNIX 系统通常用该信号来标识一个依赖于实现的硬件错误
- `SIGFPE` ：该信号因特定类型的算法错误而产生，比如：除以 0
- `SIGHUP` ：当终端断开(挂机)时，将发送该信号给终端控制进程
- `SIGILL`：试图执行非法，即格式不正确的机器语言指令，系统将向进程发送该信号
- `SIGINFO`：该信号与 `SIGPWR` 信号同义
- `SIGINT`：用户终端输入中断字符 `control+c` ，终端驱动程序将发送该信号给前台进程组，该信号的默认行为是终止进程
- `SIGIO`：利用 `fcntl()`  系统调用，可于特定类型(诸如终端和套接字)的打开文件描述符发生 IO 事件时产生该信号
- `SIGTOT`：该信号与 `SIGABRT` 信号同义
- `SIGKILL`：此信号为 "必杀" 信号，处理器程序无法将其阻塞，忽略或者捕获，故而总能终止进程
- `SIGLOST`：未加以使用
- `SIGPIPE`：当某一进程试图向管道、FIFO或者套接字中写入信息时，如果这些设备并无相应的阅读进程，那么系统将产生该信号
- `SIGPOLL`：该信号从 System V 派生而来，与 Linux 中的 `SIGIO`  信号同义
- `SIGPROF`：由 `setitimer()` 调用所设置的性能分析定时器刚一过期，内核就将产生该信号
- `SIGPWR`：电源故障信号
- `SIGQUIT`：用户在 终端输入 `control+\` ，该信号静发送到前台进程组，默认情况下，该信号终止进程，并生成可核心转储文件
- `SIGSEGV`：对内存引用无效时，产生该信号，例如：要引用的页不存在，试图修改只读内存，用户态访问内核态内存
- `SIGSTKFLT`：协处理器栈错误，Linux 对该信号作了定义，但未加以使用
- `SIGSTOP`：这是一个必停信号，处理器无法将其阻塞、忽略或者捕获，故而总能停止进程
- `SIGSYS`：如果进程发起的系统调用有误，那么将产生该信号
- `SIGTERM`：终止进程的标准信号，也是 `kill`  和 `killall` 命令的默认信号
- `SIGTRAP`：实现断点调试功能以及 `strace` 命令所执行的跟踪系统调用功能
- `SIGTSTP`：作业控制的停止信号，在终端输入 `control+z` 时，将该信号送给前台进程组，使其停止运行
- `SIGTTIN`：在作业控制 shell 下运行，若后台进程组试图对终端进行 `read()` 操作，终端驱动程序将向该进程组发送该信号，该信号默认将停止进程
- `SIGTTOU`：与 `SIGTTIN`  类似，但针对的是后台终端输出，如果终端启用了 `TOSTOP` 选项，而某一后台进程组视图对终端进行 `write()` 操作，终端驱动程序将向该进程组发送 `SIGTTOUT` 信号，该信号默认停止进程
- `SIGNUSED`：该信号没有使用
- `SIGURG`：系统发送该信号给一个进程，表示调节字存在带外数据
- `SIGUSR1`：用于自定义信号使用，内核绝不会为进程产生这个信号，进程可以使用这个信号来相互通知事件的发生，或是彼此同步
- `SIGUSR2`：用于自定义信号使用，内核绝不会为进程产生这个信号，进程可以使用这个信号来相互通知事件的发生，或是彼此同步
- `SIGVTALRM`：调用 `setitimer()`  设置的虚拟定时器刚一到期，内核就会产生该信号
- `SIGWINCH`：在窗口环境中，当终端窗口尺寸发生变化时会想前台进程组发送该信号
- `SIGXCPU`：当进程的 CPU 时间超出对应的资源限制时，将发送该信号给进程
- `SIGXFSZ`：如果试图增大文件而突破了对进程文件大小的资源限制，那么将发送该信号给进程

![](./img/signals.png)

# 改变信号处置

```
#include <signal.h>

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```

- `signum` 表示希望处理的信号编号，`handler` 表示信号抵达时所调用处理函数的地址

- 指定 `handler` 参数时可以使用下面的值来代替函数地址：

  - `SIG_DFL` ： 将该信号重置为默认值
  - `SIG_IGN`：忽略该信号
  
- 调用成功，返回的是之前的信号处置，调用失败，将返回 `SIG_ERR`
# 信号处理器简介

调用信号处理器程序，可能会随时打断主程序流程，内核代表进程来调用处理器程序，当处理器返回时，主程序会在处理器打断的位置恢复执行。

![](./img/signal_process.png)  

# 发送信号

```
#include <sys/types.h>
#include <signal.h>

int kill(pid_t pid, int sig);
```

- `pid` 标识一个或多个目标进程：
  - `pid > 0` ：发送信号给 `pid` 指定的进程
  - `pid == 0` ：发送信号给调用进程的同组的每一个进程，包括调用进程本身
  - `pid < 0` ：发送信号给：调用进程有权将信号发往的每个目标进程，除去  `init` 进程和调用进程本身
- `sig` 表示要发送的信号
- 如果没有进程与指定 `pid`  匹配，将调用失败，设置错误 `ESRCH`
- 进程要发送信号给另一进程，还需要适当的权限，其规则如下：
  - 特权级进程可以向任何进程发送信号
  - 以 `root`  用户和组运行的 `init` 进程，是一个特例，仅能接收已安装了处理器函数的信号，这可以防止系统管理员以外杀死 `init` 进程
  - 如果发送者的实际或有效用户 ID 匹配于接受者的实际用户 ID 或者保存设置用户 ID，那么非特权进程也可以向另一进程发送信号
  - `SIGCONT` 信号需要特殊处理，无论对用户 ID 的检查结果如何，非特权进程可以向同一会话中的任何其他进程发送这一信号

![](./img/send_signal_authority.png)

- 如果进程无权发送信号给所请求的 `pid`，那么 `kill()` 调用将失败，且设置 `errno` 为 `EPERM`。若 `pid` 所指为一系列进程时，只要向其中之一发送信号，则 `kill()` 调用成功

# 检查进程的存在

若将 `sig`  指定为0，即发送所谓的空信号，则无信号发送，利用这种方法可以检查特定进程 ID 的进程是否存在：

- 如果发送失败，且 `errno` 为 `ESRCH`，则表明目标进程不存在
- 如果调用失败，且 `errno` 为 `EPERM`，则表明进程存在，但实际无权向目标进程发送信号
- 如果调用成功，那么表示进程存在

特定进程 ID 的存在，不能保证指定的程序正在运行：

- 内核会随着进程的生灭而循环使用进程 ID
- 特定进程可能存在，但是一个僵尸进程，即进程已死，但其父进程尚未执行 `wait()` 来获取其终止状态

检查某一个进程是否正在运行：

- `wait()` ：监控调用者的子进程
- 信号量和排他文件锁：如果进程持续持有某一信号量或文件锁，并且一直处于被监控状态，那么如能获取到信号量或锁时，表明进程已经终止
- 管道和 FIFO 的 IPC 通道：可对监控目标进程进行设置I，令其在自身生命周期内持有对通道进行写操作的打开文件描述符，令监控进程持有针对通道进行读操作的打开文件描述符，当通道写入端关闭时，即可获知监控目标进程已终止
- `/proc/PID`：如果进程存在，则特定的 `/proc/PID`  将存在，但是由于循环使用进程ID，同一进程 ID 可能指代的是不同的程序

# 发送信号的其他方式

```
#include <signal.h>

int raise(int sig);
```

- `raise()` 使得进程向自身发送信号
- 单线程程序，调用 `raise()`  相当于： `kill(getpid(),sig)`
- 支持线程的系统将 `raise()` 实现为：`pthread_kill(pthread_self(),sig)`
- 当进程使用 `raise()` 或者  `kill()` 向自身发送信号时，信号将立即传递，即在返回调用之前
- `raise()` 出错将返回非0值，唯一失败的原因是 `EINVAL`，即 `sig` 无效

```
#include <signal.h>

int killpg(int pgrp, int sig);
```

- `pgrp()` 向某一个进程组发送一个信号，相当于 `kill(-pgrp,sig)`
- 如果 `pgrp == 0`，则将向调用者所属进程组的所有进程发送信号

# 显示信号描述

```
#define _BSD_SOURCE
#include <signal.h>

extern const char * const sys_siglist[];

#define _GNU_SOURCE
#include <string.h>

char *strsignal(int sig);
```

- `strsignal()` 对 `sig` 进行边界检查，返回一个指针，指向该信号的可打印描述字符串，或者当参数无效时，返回指向错误的字符串

```
#include <signal.h>

void psignal(int sig, const char *msg);
```

- `psignal()` 在标准错误设备上展示 `msg`  给定的字符串，后面跟一个冒号，随后是对 `sig` 信号的描述

# 信号集

多个信号可以使用一个称为信号集的数据结构来描述，数据类型为 `sigset_t`。

```
#include <signal.h>

int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
```

- `sigemptyset()` 初始化一个未包含任何成员的信号集
- `sigfillset()` 初始化一个信号集，使其包含所有信号，包含所有实时信号
- 必须使用 `sigemptyset()` 或者 `sigfillset()` 初始化信号集

```
#include <signal.h>

int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
```

- `sigaddset()` 向信号集中添加信号
- `sigdelset()` 从信号集中删除信号

```
#include <signal.h>

int sigismember(const sigset_t *set, int signum);
```

- `sigismember()` 用来测试信号 `sig`  是否是信号集 `set` 的成员，如果是，返回1，如果不是，返回 0

```
#define _GNU_SOURCE
#include <signal.h>

int sigisemptyset(const sigset_t *set);
int sigorset(sigset_t *dest, const sigset_t *left,const sigset_t *right);
int sigandset(sigset_t *dest, const sigset_t *left,const sigset_t *right);
```

- `sigandset()` 将 `left` 集和 `right`  集的交集置于  `dest`
- `sigandset()` 将 `left` 集和 `right`  集的并集置于  `dest`
- 若 `set` 集内未包含信号，则 `sigisemptyset()` 返回 `true`

# 信号掩码

内核会为每一个进程维护一个信号掩码，即一组信号，并将阻塞其针对该进程的传递，直到其被从进程信号掩码中移除。

信号掩码实际上是线程属性，在多线程进程中，每个线程都可以使用 `pthread_sigmask()` 函数来独立检查和修改其信号掩码。

向信号掩码中添加一个信号：

- 当调用信号处理器程序时，可将引发调用的信号自动添加到信号掩码中，是否发生这一情况，要视 `sigaction()`  函数在安装信号处理器程序时所使用的标志而定
- 使用 `sigaction()` 函数建立信号处理器程序时，可以指定一组额外信号，当调用该处理器程序时会将其阻塞
- 使用 `sigprocmask()` 函数可以随时向信号掩码中添加或者移除信号

```
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```

- `how` 指定了 `sigprocmask`  想给掩码带来的变化：
  - `SIG_BLOCK`：将 `set`指向信号集内的指定信号添加到信号掩码中，换言之，将信号掩码设置为其当前值和 `set` 的并集
  - `SIG_UNBLOCK`：将 `set` 指向信号集的信号从信号掩码中移除，即使要解除阻塞的信号当前并未处于阻塞状态，也不会返回错误
  - `SIG_SETMASK`：将 `set` 指向的信号集赋给信号掩码
- `oldset` 不为空，则将返回之前的信号掩码
- 如果想要获取信号掩码而又不对其改动，可以将 `set` 设置为空，此时将忽略 `how` 参数

# 处于等待状态的信号

如果进程接受了一个该进程正在阻塞的信号，会将该信号添加到进程的等待信号集中。之后如果解除了对该信号的锁定，会随即将信号传递给进程。

```
#include <signal.h>

int sigpending(sigset_t *set);
```

- `sigpending()` 返回处于等待状态的信号集于 `set`  中
- 如果修改了对等待信号的处置，那么当后来解除对信号的锁定时，将根据新的处置来处理信号

# 不对信号进行排队处理

等待信号集只是一个掩码，仅表明一个信号是否发生，而未表明其发生的次数，如果同一信号在阻塞状态下多次产生，那么会将该信号记录在等待信号集中，并在解除阻塞后只发送一次。

# 改变信号处置

除了 `signal()` 之外，还可以使用 `sigaction()` 设置信号处置。

```
#include <signal.h>

int sigaction(int signum, const struct sigaction *act,struct sigaction *oldact);
```

- `signum`：标识想要获取或改变的信号编号，可以是 `SIGKILL` 和 `SIGSTOP` 之外的任何信号
- `act`：描述信号新处置的数据结构，如果只对信号的现有处置感星期，可将该参数指定为 `NULL`
- `oldact` ：返回之前信号处置的相关信息，如果不感兴趣，则可指定为 `NULL`
- `struct sigaction`

```
struct sigaction {
    void     (*sa_handler)(int);
    void     (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t   sa_mask;
    int        sa_flags;
    void     (*sa_restorer)(void);
};
```

- `sa_handler`：指定信号处理器函数的地址，也可以是 `SIG_IGN` 或者  `SIG_DFL`
- 仅当  `sa_handler` 是信号处理程序的地址时，才会对 `sa_mask` 和 `sa_flags`  字段加以处理
- `sa_restorer` 仅供内部使用，用以确保当信号处理器程序完成后，会去调用专用的 `sigreturn()`  系统调用，借此来恢复进程的执行上下文，以便进程从信号处理器中断的位置继续执行
- `sa_mask`：定义了一组信号，在调用由 `sa_handle` 定义的处理器程序时将阻塞该信号，当调用信号处理器程序时，会在调用信号处理器之前，将该组信号中当前未处于进程掩码之列的任何信号自动添加到进程掩码中，这些信号将保留在进程掩码中，直至信号处理器函数返回，届时将自动删除这些信号



# 等待信号

```
#include <unistd.h>

int pause(void);
```













  

  

  

















