# 间隔定时器

```
#include <sys/time.h>

int setitimer(int which,const struct itimerval* new_value,struct itimerval* old_value);
```

- `setitimer()` 创建一个间隔式定时器，这种定时器会在未来某个时间点到期，并于此后(可选择地)每间隔一段时间到期一次
- `which` 可以指定以下值：
  - `ITIMER_REAL` ：创建以真实时间倒计时的定时器，到期会产生 `SIGALARM`  信号并发送给进程
  - `ITIMER_VIRTUAL`：创建以进程虚拟时间(用户模式下的 CPU 时间) 倒计时的定时器，到期时会产生信号 `SIGVTALRM`
  - `ITIMER_PROF`：创建一个 `profiling` 定时器，以进程时间(用户态与内核态 CPU 时间的总和)倒计时，到期时，则会产生 `SIGPROF` 信号

针对所有这些信号的默认处置均会终止进程，除非真地期望如此，否则就需要针对这些定时器信号创建处理器函数。

```
struct itimerval{
	struct timeval it_interval;	/* Interval for periodic timer */
	struct timeval it_value;	/* Current value(time until next expiration) */
};

struct timeval{
	time_t tv_sec;		/* Seconds */
	suseconds_t tv_usec;	/* Microseconds */
};
```

- `new_value` 下属的 `it_value` 指定了距离定时器到期的延迟时间，`it_interval` 则说明该定时器是否是周期性定时器，如果 `it_interval` 的两个字段都是 0，那么该定时器属于 `it_value` 所指定的时间间隔后到期的一次性定时器，只要 `it_interval` 中的任一字段非0，那么在每次定时器到期之后，都会将定时器重置为在指定间隔后再次到期
- 进程只能拥有上述3种定时器的一种，当第二次调用 `settimer()`  时，修改已有定时器的属性要符合参数 `which` 中的类型，如果调用 `setitimer()` 时将 `new_value.it_value` 的两个字段均设置为 0，那么会屏蔽任何已有的定时器
- 若 `old_value` 不为 `NULL`，则以其所指向的 `itimerval` 结构来返回定时器的前一设置：
  - 如果 `old_value.it_value` 的两个字段值均为 0，那么该定时器之前被设置处于屏蔽状态
  - 如果 `old_value.it_interval` 的两个字段值均为 0，那么该定时器之前被设置为历经 `old_value.it_value` 指定时间到期的一次性定时器
  - 对需要在新定时器到期后将其还原的情况而言，获取定时器的前一设置就很重要，如果不关心定时器的前一设置，可以将 `old_value` 设置为 `NULL`
- 定时器会从初始值 `it_value` 倒计时一直到 0 为止，递减为 0 时，会将相应信号发送给进程，随后，如果时间间隔值 `it_interval` 非0，那么会再次将 `it_value` 加载到定时器，重新开始向 0 倒计时

可以在任何时刻调用 `getitimer()`，以了解定时器的当前状态，距离下次到期的剩余时间：

```
#include <sys/time.h>

int getitimer(int which,struct itimerval* curr_value);
```

- `getitimer()` 返回由  `which` 指定定时器的当前状态，并置于 `curr_value` 指向的缓冲区中

使用 `setitimer()` 和 `alarm()` 创建的定时器可以跨越 `exec()` 调用而得以保存，但由  `fork()` 创建的子进程并不继承该定时器。

## 更为简单的定时器接口：`alarm()`

```
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
```

- `seconds` 表示定时器到期的秒数，到期时向调用进程发送 `SIGALRM`  信号
- 调用 `alarm()` 会覆盖对定时器的前一个设置，调用 `alarm(0)` 可以屏蔽现有定时器
- 返回值是定时器前一设置距离到期的剩余描述，如果之前并无设置，则返回 0

## `setitimer()` 和 `alarm()` 之间的交互

Linux 中 `alarm()` 和 `setitimer()`  针对同一进程共享一个实时定时器，无论调用两者之中的哪个完成了对定时器的前一设置，同样可以调用二者中的任一函数来改变这一设置。

程序设置实时定时器时，最好选用二者之一。

# 定时器的调度和精度

内核配置项 `CONFIG_HIGH_RES_TIMERS` 可以支持高分辨率定时器，使得定时器的精度不受软件时钟周期的影响，可以达到底层硬件所支持的精度，在现代硬件平台上，精度达到微秒级别是司空见惯的。

# 为阻塞操作设置超时

实时定时器的用途之一就是为某个阻塞系统调用设置其处于阻塞状态的时间上限。

例如，处理 `read()` 操作：

- 调用 `sigaction()` 创建  `SIGALRM` 信号的处置函数，排除 `SA_RESTART` 标志以确保系统调用不会重新启动
- 调用 `alarm()` 或者 `setitimer()` 创建定时器，设置超时时间
- 执行阻塞的系统调用
- 系统调用返回，再次调用 `alarm()` 或 `setitimer()` 屏蔽定时器
- 检查系统调用失败是否设置 `errno` 为 `EINTR` ，即系统调用遭到中断

# 暂停运行一段固定时间

## 低分辨率休眠：`sleep()`

```
#include <unistd.h>

unsigned int sleep(unsigned int seconds);
```

- `sleep()` 可以暂停调用进程执行  `seconds` 秒，或者在捕获信号后恢复进程的执行
- 如果休眠正常结束，返回0，如果因信号中断休眠，返回剩余的秒数
- 考虑到一致性，应该避免 `sleep()` 和 `alarm()` 以及 `setitimer()` 之间的混用，Linux 将 `sleep()` 实现为对 `nanosleep()` 的调用，而有些老系统使用 `alarm()` 和  `SIGALRM` 信号处理函数实现 `sleep()`

## 高分辨率休眠 `nanosleep()`

```
#include <time.h>

int nanosleep(const struct timespec *req, struct timespec *rem);
```

- `nanosleep()` 与 `sleep()` 相似，但是分辨率更高
-  `struct timespec`：

```
struct timespec {
        time_t tv_sec;        /* seconds */
        long   tv_nsec;       /* nanoseconds */
};
```

- 规范规定不得使用信号实现该函数，这意味着 `nanosleep()` 与 `alarm()` 和  `setitimer()` 混用，也不会危及程序的可移植性
- 尽管 `nanosleep()` 没有使用信号，但还是可以通过信号处理器函数将其中断，此时将返回 -1，并设置错误 `EINTR`，如果 `remain` 不为 `NULL`，则该指针所指向的缓冲区将返回剩余的休眠时间，可以利用这个返回值重启该系统调用以完成休眠，但是由于返回的 `remain` 时间未必是软件时钟间隔的整数倍，故而每次重启都会遭受取整，其结果是，每次重启后的休眠时间都要长于前一调用返回的 `remain` 值，在信号接收频率很高的情况下，进程的休眠可能永远也结束不了，使用 `TIMER_SBSTIME` 选项的 `clock_nanosleep()` 可以避免这个问题

# POSIX 时钟

Linux 中需要使用 realtime，实时函数库，需要链接 `librt` 即需要加入 `-lrt` 选项。

## 获取时钟的值





 









 































