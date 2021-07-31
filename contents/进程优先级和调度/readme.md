# 进程优先级(nice 值)

Linux 调度进程使用 CPU 的默认模型是循环时间共享，每个进程轮流使用 CPU 一段时间，这段时间被称为时间片或量子，这种模型满足：

- 公平性：每个进程都有机会使用到 CPU
- 响应性：一个进程在使用 CPU 之前无需等待太长时间

循环时间共享法特点：

- 进程无法直接控制何时使用 CPU 以及使用 CPU 的时间
- 每个进程轮流使用 CPU 直至时间片被用光或自己自动放弃 CPU (如进行睡眠或者执行一个磁盘读取操作)
- 如果所有进程都试图尽可能多地使用 CPU，即没有进程睡眠或被 IO 阻塞，那么它们使用 CPU 的时间差不多是相等的

进程特性 nice 值允许进程间接地影响内核调度算法，每个进程都有一个  nice 值，其范围 -20 ~ 19，默认值为 0。

![](./img/nice.png)

使用 `fork()`  创建子进程时会继承 nice 值并且该值会在 `exec()` 调用中得以保持。

## nice 值得影响

进程的调度不是严格按照 nice 值得层次进行的，nice 值是一个权重因素，它导致内核调度器倾向于调度拥有高优先级的进程。

给一个进程赋一个低优先级 nice 值并不会导致它完全无法用到 CPU，但会导致它使用 CPU 的时间变少。

nice 值对进程调度的影响程度则依据 Linux 内核版本的不同而不同。

## 获取和修改优先级

```
#include <sys/time.h>
#include <sys/resource.h>

int getpriority(int which, id_t who);
int setpriority(int which, id_t who, int prio);
```

- `which` 确定 `who` 参数如何被解释，其取值为：
  - `PRIO_PROCESS`：操作进程 ID 为 `who` 进程，如果 `who` 为 0，那么使用调用者的进程 ID
  - `PRIO_PGRP`：操作进程组 ID 为 `who` 的进程组中的所有成员，如果 `who` 为 0，那么使用调用者的进程组
  - `PRIO_USER`：操作所有真实用户 ID 为 `who` 的进程，如果 `who`  为 0，那么使用调用者的真实用户 ID
- `getpriority()` 返回由 `which` 和 `who` 指定的进程 nice 值，如果有多个进程符合指定的标准(当 `which` 为 `PRIO_PGRP` 或 `PRIO_USER` 时)，那么将返回优先级最高的进程 nice 值(即最小的数值)，`getpriority()` 成功时可能返回 -1，因此在调用这个函数之前要将 `errno` 设置为0，接着在调用之后检查返回值为-1以及 `errno` 不为0才能确认调用成功
- `setpriority()` 试图将 nice 值设置为一个超出范围(-20 ~ 19) 的 nice 值时，将直接设置为边界值

特权进程 `CAP_SYS_NICE` 能够修改任意进程的优先级，非特权进程可以修改自己的优先级和其他进程的优先级，前提是自己的有效用户 ID 与目标进程的真实或有效用户 ID 匹配。

非特权进程能够将自己的 nice 值最高提高到公式 `20-rlim_cur` 指定的值，`rlim_cur` 是当前的 `RLIMIT_NICE` 的软资源限制。

# 实时进程调度概述

实时调度策略 `SCHED_RR` 和  `SCHED_FIFO` 优先级要高于标准循环时间分享策略(`SCHED_OTHER`)来调度的进程。

Linux 提供 99 个实时优先级，其数值从 1 ~ 99，并且这个取值范围同时适用于两个实时调度策略。每个策略中的优先级是等价的，这意味着如果两个进程拥有同样的优先级，一个进程采用了 `SCHED_RR` 策略，一个采用了 `SCHED_FIFO` 策略，那么两个都符合运行的条件，至于到底运行哪个则取决于它们被调度的顺序。

每个优先级级别都维护着一个可运行的进程队列，下一个运行的进程是从优先级最高的非空队列的队头选取出来的。

## SCHED_RR  策略

`SCHED_RR`  策略，优先级相同的进程以循环时间分享得方式执行，进程每次使用 CPU 的时间为一个固定长度的时间片，一旦被调度执行之后，使用 `SCHED_RR` 策略的进程会保持对 CPU 的控制直到下列条件中的一个得到满足：

-  到达时间片的终点了
- 自愿放弃 CPU，这可能是因为执行了一个阻塞式系统调用或调用了 `sched_yield()` 系统调用
- 进程终止了
- 被一个优先级更高的进程抢占了

对于上面四种情况，前两个情况导致进程丢掉 CPU 之后将会被放置在与其优先级级别对应的队列的队尾，在最后一种情况下，当优先级更高的进程执行结束之后，被抢占的进程会继续执行直到其时间片的剩余部分被消耗完(即被抢占的进程仍然位于与其优先级级别对应的队列的队头)。

在 `SCHED_RR` 和 `SCHED_FIFO` 两种策略中，当前运行的进程可能会因为下面某个原因被抢占：

- 之前被阻塞的高优先级进程解除阻塞了(如它所等待的 IO 操作完成了)
- 另一个进程的优先级被提到了一个级别高于当前运行的进程的优先级的优先级
- 当前运行的进程的优先级被降低到低于其他可运行的进程的优先级

`SCHED_RR` 和 `SCHED_OTHER` 类似，即它允许优先级相同的一组进程分享 CPU 时间，它们之间最重要的差别在于 `SCHED_RR` 策略存在严格的优先级级别，高优先级的进程总是优先于低优先级较低的进程。而在 `SCHED_OTHER` 策略中，低 nice 值得进程不会独占 CPU，它仅仅在调度决策时为进程提供了一个较大的权重。另一个重要的区别是 `SCHED_RR` 策略允许精确控制进程被调用的顺序。

## SCHED_FIFO 策略

`SCHED_FIFO ` 先入先出的策略与 `SCHED_RR` 相类似，它们之间最主要的差别在于 `SCHED_FIFO ` 策略中不存在时间片，一旦一个 `SCHED_FIFO ` 进程获得了 CPU 的控制权之后，它就会一直执行直到下面某个条件满足：

- 自动放弃 CPU
- 终止了
- 被一个优先级更高的进程抢占了

上面第一种情况中，进程会被放置在与其优先级级别对应的队列的队尾，在最后一种情况中，当高优先级进程执行了结束之后，被抢占的进程会继续执行，即被抢占的进程位于与其优先级级别对应的队列的队头。

## SCHED_BATCH 和 SCHED_IDLE 策略

Linux 2.6 内核增加了两个非标准调度策略：`SCHED_BATCH` 和 `SCHED_IDLE`。

- `SCHED_BATCH` 策略与默认的 `SCHED_OTHER` 策略类似，但是 `SCHED_BATCH` 策略会导致频繁被唤醒的任务被调度的次数较少，这种策略用于进程的批量式执行
- `SCHED_IDLE` 策略与默认的 `SCHED_OTHER` 策略类似，但是 `SCHED_IDLE` 提供的功能等价于一个非常低的 nice 值(即低于+19)，在这个策略中，进程的 nice 值毫无意义，它用于运行低优先级的任务，这些任务在系统中没有其他任务需要使用 CPU 时才会大量使用 CPU

# 实时进程调用 API

## 实时优先级范围

```
#include <sched.h>

int sched_get_priority_max(int policy);
int sched_get_priority_min(int policy);
```

- `sched_get_priority_min()` 和 `sched_get_priority_max()` 获取一个调度策略的优先级的取值范围
- `policy` 的取值一般是 `SCHED_RR` 或者 `SCHED_FIFO`

## 修改和获取策略和优先级

### 修改调度策略和优先级

```
#include <sched.h>

int sched_setscheduler(pid_t pid, int policy,const struct sched_param *param);
```

- `sched_setscheduler()` 修改 `pid` 进程的调度策略和优先级，如果 `pid` 是 0，那么将会修改调用进程的特性
- `param` 参数：

```
struct sched_param{
	int sched_priority;
};
```

-  `policy` 指定了进程的调度策略，可以是：

![](./img/policy.png)

- 成功调用 `sched_setscheduler()` 会将 `pid` 指定的进程移到与其优先级级别对应的队列的队尾
- 通过 `fork()` 创建的子进程会继承父进程的调度策略和优先级，并且在 `exec()` 调用中会保持这些信息

```
#include <sched.h>

int sched_setparam(pid_t pid, const struct sched_param *param);
```

- `sched_setparam()` 可以修改一个进程的调度策略，但不会修改其优先级
- 成功调用 `sched_setparam()` 会将 `pid` 指定的进程移到与其优先级级别对应的队列的队尾

### 权限和资源限制会影响对调度参数的变更

特权进程(CAP_SYS_NICE) 能够随意修改任意进程的调度策略和优先级。

非特权进程也能够根据下列规则修改调度策略和优先级：

- 如果进程拥有非零的 `RLIMIT_RTPRIO` 软限制，那么它就能随意修改自己的调度策略和优先级，只要符合实时优先级的上限为其当前实时优先级的最大值及其 `RLIMIT_RTPRIO` 软限制值的约束即可
- 如果进程的 `RLIMIT_RTPRIO` 软限制值为 0，那么进程只能降低自己的实时调度优先级或从实时策略切换到非实时策略
- `SCHED_IDLE` 策略是一种特殊的策略，运行在这个策略下的进程无法修改自己的策略，不管 `RLIMIT_RTPRIO` 资源限制的值是什么
- 在其他非特权进程中也能执行策略和优先级的修改工作，只要该进程的有效用户 ID 与目标进程的真实或有效用户 ID 匹配即可
- 进程的软 `RLIMIT_RTPRIO` 限制值只能确定可以对自己调度策略和优先级做出哪些变更，这些变更可以由进程自己发起，也可以由其他非特权进程发起，拥有非零限制值的非特权进程无法修改其他进程的调度策略和优先级

### 获取调度策略和优先级

```
#include <sched.h>

int sched_getscheduler(pid_t pid);
int sched_getparam(pid_t pid, struct sched_param *param);
```

### 防止实时进程锁住系统















































































































