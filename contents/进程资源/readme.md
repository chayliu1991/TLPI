# 进程资源使用

```
#include <sys/time.h>
#include <sys/resource.h>

int getrusage(int who, struct rusage *usage);
```

- `getrusage()` 返回调用进程或其子进程用掉的各类资源的统计信息
- `who` 指定了需查询资源使用信息的进程，其取值为：
  - `RUSAGE_SELF`：返回调用进程的相关信息
  - `RUSAGE_CHILDREN`：返回调用进程的所有被终止和处于等待状态的子进程相关的信息
  - `RUSAGE_THREAD`：Linux 特有，返回调用线程的相关信息
- `rusage`：

```
 struct rusage {
               struct timeval ru_utime; /* user CPU time used */
               struct timeval ru_stime; /* system CPU time used */
               long   ru_maxrss;        /* maximum resident set size */
               long   ru_ixrss;         /* integral shared memory size */
               long   ru_idrss;         /* integral unshared data size */
               long   ru_isrss;         /* integral unshared stack size */
               long   ru_minflt;        /* page reclaims (soft page faults) */
               long   ru_majflt;        /* page faults (hard page faults) */
               long   ru_nswap;         /* swaps */
               long   ru_inblock;       /* block input operations */
               long   ru_oublock;       /* block output operations */
               long   ru_msgsnd;        /* IPC messages sent */
               long   ru_msgrcv;        /* IPC messages received */
               long   ru_nsignals;      /* signals received */
               long   ru_nvcsw;         /* voluntary context switches */
               long   ru_nivcsw;        /* involuntary context switches */
           };
```

- `ru_utime` 和 `ru_stime`  字段的类型是 `timeval` 结构，分别表示一个进程在用户模式和内核模式下消耗的 CPU 的秒数和毫秒数
- 在 `RUSAGE_CHILDREN` 操作中，`ru_maxrss` 字段返回调用进程的所有子孙进程中最大驻留集大小，不是所有子孙进程之和

# 进程资源限制

每个进程都用一组资源限值，它们可以用来限制进程能够消耗的各种系统资源。可以示使用 `ulimit` 设置  shell 的资源限制，shell 创建用来执行用户命令的进程会继承这些限制。

```
#include <sys/time.h>
#include <sys/resource.h>

int getrlimit(int resource, struct rlimit *rlim);
int setrlimit(int resource, const struct rlimit *rlim);
```

- `getrlimit()` 和 `setrlimit()` 允许一个进程读取和修改自己的资源限制
- `resource` 参数标识了需读取或修改的资源限制，`rlim` 用来指定或者返回限制值：

```
 struct rlimit {
               rlim_t rlim_cur;  /* Soft limit */
               rlim_t rlim_max;  /* Hard limit (ceiling for rlim_cur) */
           };
```

- `rlim_cur` 是资源的软限制，软限制规定了进程能够消耗的资源总量，一个进程可以将软限制调整为从 0 到硬限制之间的值
- `rlim_max` 是资源的硬限制，其作用为软限制设定上限，特权进程能够增大和缩小硬限制，非特权进程只能缩小硬限制，但是这个行为不可逆
- 当 `rlim_cur` 和 `rlim_max` 的取值为 `RLIM_INFINITY` 表示没有限制



















