System V 信号量不是用来在进程间传输数据的，而是用来同步进程的动作。信号量的一个常见用途是同步对一块共享内存的访问以防止出现一个进程在访问共享内存的同时另一个进程更新这块内存的情况。

一个信号量是一个由内核维护的整数，其值被限制为大于或者等于0。在一个信号量上可以执行各种操作(即系统调用)，包括：

- 将一个信号设置成一个绝对值
- 在信号量当前值的基础上加上一个数量
- 在信号量当前值的基础上减去一个数量
- 等待信号量的值等于 0

上面操作中的后两个可能会导致调用进程阻塞。当减小一个信号量的值时，内核会将所有试图将信号量值降低到 0 之下的操作阻塞。类似的，如果信号量的当前值不为 0，那么等待信号量的值等于 0 的调用进程将会发生阻塞。不管是何种情况，调用进程会一直保持阻塞直到其他一些进程将信号量的值修改为一个允许这些操作继续向前的值，在那个时刻内核会唤醒被阻塞的进程。下图显示了使用一个信号量来同步两个交替将信号量的值在 0 和 1 之间切换的进程的动作：

![](./img/signal_sync.png)

# 概述

使用 System V 信号量的常规步骤如下：

- 使用 `semget()` 创建或者打开一个信号量集
- 使用 `semctl() SETVAL` 或 `SETALL` 操作初始化集合中的信号量(只有一个进程需要完成这个任务)
- 使用 `semop()` 操作信号量的值。使用信号量的进程通常会使用这些操作来标识一种共享资源的获取和释放
- 当所有进程都不再需要使用信号量集之后 `semctl() IPC_RMID` 操作删除这个集合(只有一个进程需要完成这个任务)

大多数操作系统都为应用程序提供了一些信号量原语。但 System V 信号量表现出了不同寻常的复杂性，因为它们的分配是以被称为信号量集的组为单位进行的。在使用 `semget()` 系统调用创建集合的时候需要指定集合中的信号量数量。虽然同一时刻通常只会操作一个信号量，但通过 `semop()` 系统调用可以原子的在同一个集合中的多个信号量之上执行一组操作。

# 创建或者打开一个信号量集

```
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semget(key_t key, int nsems, int semflg);
```

- `semget()` 系统调用创建一个新信号量集或者获取一个既有集合的标识符
- 如果 `semget()` 创建一个新信号集，那么 `nsems` 会指定集合中信号量的数量，并且其值必须大于 0，如果 `semget()` 是获取一个既有集的标识符，那么 `nsems` 必须小于等于集合的大小（否则会发生 `EINVAL` 错误）。无法修改一个既有集中的信号量数量
- `semflg` 是一个位掩码，它指定了施加于新信号量集之上的权限或需检查一个既有集合的权限。指定权限的方式与为文件指定权限的方式是一样的。此外，在 `semflg` 中可以通过对下列标记中的零个或多个取 `OR` 来控制 `semget()` 的操作：
  - `IPC_CREAT` ：如果不存在与指定的 `key` 相关联的信号量集，那么就创建一个新集合
  - `IPC_EXCL` ：如果同时指定了 `IPC_CREAT` 并且与指定的 `key` 关联的信号量集已经存在，那么返回 `EEXIST` 错误
- `semget()` 在成功时会返回新信号量集中既有信号量集的标识符。后续引用单个信号量的系统调用必须要同时指定信号量标识符和信号量在集合中的序号。一个集合中的信号量从0开始计数

# 信号量控制操作

```
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semctl(int semid, int semnum, int cmd, ... /* union semun arg*/);
```

- `semctl()` 系统调用在一个信号量集或集合中的单个信号量上执行各种控制操作
- `semid` 是操作所施加的信号量集的标识符，对于那些在单个信号量上执行的操作，`semnum` 标识出了集合中的具体信号量，对于其他操作则会忽略这个参数，并且可以将其设置为 0
- `cmd` 指定要执行的操作
- 一些特定的操作需要向 `semctl()` 传入第四个参数 `arg`，这个操作是一个 `union`。在程序中必须要显示的定义这个 `union`：

```
#ifndef SEMUN_H
#define SEMUN_H             

#include <sys/types.h>  
#include <sys/sem.h>

union semun {   
    int                 val;
    struct semid_ds *   buf;
    unsigned short *    array;
#if defined(__linux__)
    struct seminfo *    __buf;
#endif
};

#endi
```

## 常规控制操作

下面这些操作都会忽略 `semnum` 参数：

- `IPC_RMID`：立即删除信号量集及其关联的 `semid_ds` 数据结构，所有因在 `semop()` 调用中等待这个集合中的信号量而阻塞的进程都会立即被唤醒，`semop()` 会报告错误 `EIDRM`。这个操作无需 `arg` 参数
- `IPC_STAT` ：`arg.buf` 指向的缓冲器中放置一份与这个信号量集相关联的 `semid_ds` 数据结构的副本
- `IPC_SET`：使用 `arg.buf` 指向的缓冲器中的值来更新与这个信号量集相关联的 `semid_ds` 数据结构中选中的字段

## 获取和初始化信号量值

下面的操作可以获取或初始化一个集合中的单个或所有信号量的值，获取一个信号量的值需具备在信号量上的读权限，而初始化该值则需要修改 (写) 权限。

- `GETVAL`：`semctl()` 返回由 `semid` 指定的信号量集中第 `semnum个` 信号量的值。这个操作无需 `arg` 参数
- `SETVAL`：将由 `semid` 指定的信号量集中第 `semnum` 个信号量的值初始化为 `arg.val`
- `GETALL`：获取由 `semid` 指向的信号量集中所有信号量的值并将它们放在 `arg.array` 指向的数组中。必须要确保该数组具备足够的空间(通过由 `IPC_STAT` 操作返回的 `semid_ds` 数据结构中的 `sem_nsems` 字段可以获取集合中的信号量数量)。这个操作将忽略 `semnum` 参数
- `SETALL`：使用 `arg.array` 指向的数组中的值初始化 `semid` 指向的集合中的所有信号量。这个操作将忽略 `semnum`  参数

如果存在一个进程正在等待在由 `SETVAL`  或 `SETALL`  操作所修改的信号量上执行一个操作并且对信号量所做的变更将允许该操作继续向前执行，那么内核就会唤醒该进程。

使用 `SETVAL`  或 `SETALL` 修改一个信号量的值会在所有进程中清除该信号量的撤销条目。

注意 `GETVAL` 和 `GETALL ` 返回的信息在调用进程使用它们时可能已经过期了。所有依赖由这些操作返回的信息保持不变这个条件的程序都可能会遇到检查时（time-of-check）和使用时（time-of-use）的竞争条件。

## 获取单个信号量的信息

- `GETPID` 返回上一个在该信号量上执行 `semop()` 的进程的进程 ID；这个值被称为 `sempid` 值，如果还没有进程在该信号量上执行过 `semop()`，那么就返回 0
- `GETNCNT` 返回当前等待该信号量的值增长的进程数；这个值被称为 `semncnt` 值
- `GETZCNT` 返回当前等待该信号量的值变成 0 的进程数；这个值被称为 `semzcnt` 值

与上面介绍的 `GETVAL` 和 `GETALL` 操作一样，`GETPID`、`GETNCNT` 以及 `GETZCNT` 操作返回的信息在调用进程使用它们时可能已经过期了。

#  信号量关联数据结构

每个信号量集都有一个关联的 `semid_ds` 数据结构，其形式如下：

```
struct semid_ds
{
    struct ipc_perm sem_perm;		/* operation permission struct */
    time_t sem_otime;			    /* last semop() time */
    time_t sem_ctime;			    /* last time changed by semctl() */
    unsigned long sem_nsems;		/* number of semaphores in set */
};

struct ipc_perm
{
    __key_t __key;			/* Key.  */
    __uid_t uid;			/* Owner's user ID.  */
    __gid_t gid;			/* Owner's group ID.  */
    __uid_t cuid;			/* Creator's user ID.  */
    __gid_t cgid;			/* Creator's group ID.  */
    unsigned short int mode;		/* Read/write permission.  */
    unsigned short int __pad1;
    unsigned short int __seq;		/* Sequence number.  */
    unsigned short int __pad2;
};
```

各种信号量系统调用会隐式地更新 `semid_ds` 结构中的字段，使用 `semctl() IPC_SET` 操作能够显式地更新 `sem_perm` 字段中的特定子字段：

- `sem_perm` 创建信号量时会初始化这个子结构中的字段。通过 `IPC_SET` 能够更新 `uid`、`gid`、`mode` 子字段
- `sem_otime` 创建信号量集时会将这个字段设置为0，然后每次成功的 `semop()` 调用或者当信号量指应该 `SEM_UNDO` 操作而发生变更时将这个字段设置为当前时间，这个字段和 `sem_ctime` 的类型为 `time_t`，它们存储自新纪元到现在的秒数。
- `sem_ctime` 在创建信号量时以及每个成功的 `IPC_SET`、`SETALL` 和 `SETVAL` 操作执行完毕之后将这个字段设置为当前时间。（在一些 UNIX 实现上，`SETALL ` 和 `SETVAL`  操作不会修改 `sem_ctime`。
- `sem_nsems` 在创建集合时将这个字段的值初始化为集合中信号量的数量。

# 信号量初始化

```
/* Create a  set contaiong 1 semaphore*/
semid = semget(key,1,IPC_CREAT | IPC_EXCL | perms);
if(semid != -1)
{
	union semun arg;
	
	arg.val = 0;
	if(semctl(semid,0,SETVAL,arg) == -1)
		errExit("semctl()");
}
else
{
    if(errno != EEXIST)
        errExit("semget()");
    
    semid = semget(key,1,perms);
    if(semid == -1)
        errExit("semget()");    
}

/* Now perform some operation on the semaphore */

sops[0].sem_op = 1;
sops[0].sem_num = 0;
sops[0].sem_flg = 0;

if(semop(semid,sops,1) == -1)
    errExit("semop()");
```

上面的代码存在的问题是如果两个进程同时执行，可能发生竞争条件：

![](./img/race_condition.png)

为了确保一个集合中的多个信号量能被正确地初始化以及一个信号量被初始化为一个非零值，可以使用下面的代码：

```
semid = semget(key,1,IPC_CREAT | IPC_EXCL | perms);
if(semid != -1)
{
	union semun arg;
    struct sembuf sop;
	
	arg.val = 0;
	if(semctl(semid,0,SETVAL,arg) == -1)
		errExit("semctl()");
    
    /* perform a  "no-op" semaphore operation - changes sem_otime so other process can see we've initialized the set.*/
    
    sops[0].sem_op = 0;
    sops[0].sem_num = 0;
    sops[0].sem_flg = 0;

    if(semop(semid,sops,1) == -1)
        errExit("semop()");
}
else
{
    const int MAX_TRIES = 10;
    int j;
    union semun arg;
    struct semid_ds ds;

    if(errno != EEXIST)
        errExit("semget()");
    
    semid = semget(key,1,perms);
    if(semid == -1)
        errExit("semget()");   

    /* Wait until another process has called semop() */

    arg.buf = &ds;
    for(j = 0;j < MAX_TRIES;j++)
    {
        if(semctl(semid,0,IPC_STAT,arg) == -1)
            errExit("semctl()");
        if(ds.sem_otime != 0)
            break;
        sleep(1);
    }

    if(ds.sem_otime == 0)
        errExit("Existing semaphore not initialized");

}

/* Now perform some operation on the semaphore */
```



























3















































































