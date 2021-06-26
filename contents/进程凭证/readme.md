每个进程都有一套数字表示的用户 ID(UID) 和组 ID(GID)，也将这些 ID 称为进程凭证：

- 实际用户 ID(real user ID) 和实际组 ID(real group ID)
- 有效用户 ID(effective user ID) 和有效组 ID(effective group ID)
- 保存的 set-user-ID(saved set-user-ID) 和保存的 set-group-ID(saved set-group-ID)
- 文件系统用户 ID(file-system user ID) 和文件系统组 ID(file-system groupID)
- 辅助组 ID

# 实际用户 ID 和实际组 ID

实际用户 ID 和实际组 ID 确定了进程所属的用户和组：

- 登录 shell 从 `/etc/passwd` 文件中读取相应的信息，将其置为实际用户 ID 和实际组 ID
- 创建新进程时，将从父进程继承实际用户 ID 和实际组 ID

# 有效用户 ID 和有效组 ID

当进程试图执行各种系统调用时，将结合有效用户 ID，有效组 ID，连同辅助组 ID 一起来确定授予进程的权限。

有效用户 ID 为  0 的进程拥有超级用户的所有权限，这样的进程称为特权级进程，某些系统调用只能由特权级进程执行。

通常有效用户 ID 和组 ID 与其相应的实际 ID 是相同的，但是也可以修改：

- 使用系统调用进行修改
- 使用 set-user-ID  和 set-group-ID 进行修改

# Set-User-ID 和 Set-Group-ID 程序

Set-User-ID 将进程的有效用户 ID 设置为可执行文件的用户 ID，从而获得常规情况下并不具有的权限。Set-Group-ID 对进程的有效组 ID 实现类似的任务。

可执行文件的用户 ID 和 组 ID决定了该文件的所有权，可执行文件还拥有两个特别的权限位 se-user-ID 和 set-group-ID。可使用 `chmod` 命令来设置这些权限位，非特权用户能够对其拥有的文件进行设置，而特权级用户能够对任何文件进行设置。

```
ls -l prog
-rwxr-xr-x 1 root root 30275 Jun 26 15:06 prog
chmod u+s prog	# 打开 set-user-ID 权限位
chmod g+s prog	# 打开 set-group-ID 权限位
```

如果设置了 se-user-ID 和 set-group-ID 权限位，那么通常用来表示文件可执行权限的 `x` 会被标识为 `s`。

```
ls -l prog
-rwsr-sr-s 1 root root 30275 Jun 26 15:06 prog
```

当运行设置了  set-user-ID 的程序时，内核会将进程的有效用户 ID 设置为可执行文件的用户 ID。set-group-ID 的作用与之类似。

通过这种方法修改进程的有效用户 ID 和有效组 ID 能够使进程获取常规情况下不具备的权限。

经常使用 set-user-ID 的程序：

-  `passwd` ： 用于修改用户密码
- `mount` ：加载文件系统
- `umount` ： 卸载文件系统
- `su` ： 允许用户以另一用户的身份执行 shell

经常使用 set-group-ID 的程序：

- `wall` ： 向 tty 组下辖的所有终端写入一条信息

# 保存 Set-User-ID 和 保存 Set-Group-ID

保存 Set-User-ID 和 保存 Set-Group-ID 与 Set-User-ID 和 Set-Group-ID 结合使用，当程序执行是 ，依次发生如下事件：

- 若可执行文件的 Set-User-ID(Set-Group-ID) 权限位已经开启，则将进程的有效用户(组) ID 设置为可执行文件的属主，如果未设置  Set-User-ID(Set-Group-ID) 权限位，则进程的有效用户(组)  ID将保持不变
- 保存 Set-User-ID 和 保存 Set-Group-ID 由对应的有效 ID 复制而来，无论正在执行的文件是否设置了 Set-User-ID 和 Set-Group-ID，这一复制都将进行

假设某个进程的实际用户 ID、有效用户 ID 和保存 Set-user-ID 都是 1000，当执行了 root 用户拥有的 Set-user-ID 程序后，进程的用户 ID 将发生如下变化：

```
real=1000 effective=0 saved=0
```

有不少系统调用，允许将 Set-user-ID(Set-group-ID) 程序的有效用户 ID 在实际用户(组) ID 和保存  Set-user-ID(Set-group-ID) 之间切换。

# 文件系统用户 ID 和组ID

要进行诸如打开文件，改变文件属主，修改文件权限之类的文件系统操作时，决定其操作权限的是文件系统用户 ID 和组 ID(结合辅助组ID)，而非有效用户 ID 和组 ID。

通常，文件系统用户 ID 和组 ID 的值等同于相应的有效用户 ID 和组ID，因而一般也等同于相应的实际用户 ID 和组ID。

只要有效用户或组 ID 发生了变化，则相应的文件系统 ID 也将随之变化为同一值，只有当使用 `setfsuid()`  和 `setfsgid()` 时，才可以刻意制造出文件系统 ID 与相应的有效 ID 的不同。

# 辅助组 ID 

辅助组 ID 用于标识进程所属的若干附属组，新进程从其父进程继承这些 ID，登录 shell 将从系统组文件中获取其辅助组  ID，将这些 ID 与有效 ID 以及文件系统 ID 相结合，就能决定对文件、System V IPC 对象和其它系统资源的访问权限。

# 获取和修改进程凭证

可以利用 `/proc/PID/status` 中的信息获取任何进程凭证。

Linux 超级用户权限有多种各不相同的能力，修改用户 ID 和 组 ID 的所有系统调用涉及的权限：

- `CAP_SETUID` ：允许进程任意修改其用户 ID
- `CAP_SETGID` ： 允许进程任意修改其组 ID

## 获取和修改实际、有效、保存设置标识

### 获取实际和有效 ID

```
#include <unistd.h>
#include <sys/types.h>

uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
```

- 这些系统调用总是会成功

### 修改有效  ID

```
#include <sys/types.h>
#include <unistd.h>

int setuid(uid_t uid);
int setgid(gid_t gid);
```

- 规则1：非特权进程调用 `setuid()` 时，仅能修改进程的有效用户 ID，并且仅能将有效用户 ID 修改成相应的实际用户 ID 或保存 Set-usr-ID
- 规则2：特权进程以一个非 0 参数调用  `setuid()`  时，其实际用户 ID、有效用户 ID 和保存 Set-user-ID 都被设置为 `uid`  指定的参数，这个操作是单向的，一旦特权进程以此方式修改了其 ID，那么所有特权都将丢失，且之后也能使用 `setuid()` 调用将有效用户 ID 重置为 0
- 使用 `setgid()`  系统调用修改组 ID 的规则与 `setuid()` 类似，但在规则2中，由于对组 ID 的修改不会引起进程特权的丢失(拥有特权与否由有效用户 ID 决定)，特权级程序可以使用 `setgid()` 对组 ID 进行任意修改

对 set-user-ID-root 的程序(其有效用户 ID 的当前值为 0)，以不可逆方式放弃进程所有特权的首选方法是以实际用户 ID 来设置有效用户 ID 和保存 set-user-ID：

```
if(setuid(getuid()) == -1)
	errExit("setuid");
```

只修改有效用户(组) ID：

```
#include <sys/types.h>
#include <unistd.h>

int seteuid(uid_t euid);
int setegid(gid_t egid);
```

- 规则1：非特权级进程仅能将其有效 ID 修改为其相应的实际 ID 或者保存设置 ID
- 规则2：特权进程能够将其有效 ID 修改为任意值，若特权进程使用 `seteuid()`  将其有效用户 ID 修改为非 0 值，那么此进程将不再具有特权，但是可以根据规则1恢复特权

对于需要对特权收放自如的  Set-user-ID 和 Set-group-ID 的 程序，推荐使用 `seteuid()`：

```
euid = geteuid();
if(seteuid(getuid() == -1)
	errExit("seteuid");
if(setuid(euid) == -1)
	errExit("seteuid");
```

- `setegid()` 的规则与之类似

### 修改实际 ID 和有效 ID

```
#include <sys/types.h>
#include <unistd.h>

int setreuid(uid_t ruid, uid_t euid);
int setregid(gid_t rgid, gid_t egid);
```

- `setreuid()` 允许调用进程独立修改其实际和有效 ID
- `setregid()` 允许调用进程独立修改其实际和有效组 ID
- 如果只想修改其中一个 ID，可以将另一个参数指定为 -1
- `setreuid()` 的规则：
  - 规则1：非特权进程只能将其实际用户 ID 设置为当前实际用户 ID 的值(即保持不变)或有效用户 ID 值，其只能将有效用户 ID 设置为当前实际用户 ID或者有效用户ID(即保持不变)或者保存 set-user-ID
  - 规则2：特权级进程能够设置其实际用户 ID 和有效用户 ID 为任意值
  - 规则3：不管进程是否拥有特权，只要下面的条件之一成立，就能将保存 set-user-ID 设置成新的有效用户 ID：
    - ruid 不为 -1，即设置实际用户 ID，即便是置为当前值
    - 对有效用户 ID 所设置的值不同于系统调用之前的实际用户 ID
- `setregid()` 规则与 `setreuid()` 相类似
- `setregid()` 和 `setreuid()` 调用具有 0/1 效应，即对 ID 的修改请求，要么全成功，要么全失败

set-user-ID-root 进程如果有意将用户凭证和组凭证改变为任意值，则应首先调用 `setrugid()`  再调用 `setreuid()` ，一旦调用顺序颠倒了，调用 `setrugid()` 将会失败，因为调用 `setrugid()`  后，程序不再具有特权。

### 获取实际、有效、保存设置 ID

```
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <unistd.h>

int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);
```

- 若不想修改某个 id 将参数设置为 -1
- `getresuid()` 的规则：
  - 非特权进程能够将实际用户 ID、有效用户 ID 和保存 Set-user-ID 中的任一设置为实际用户 ID，有效用户 ID 或者保存 Set-user-ID 之中的任一当前值
  - 特权级进程能够对其实际用户 ID，有效用户 ID 和保存 Set-user-ID 做任意设置
  - 不管系统调用是否对其他 ID 做了任何改动，总是将文件系统用户 ID 设置为其有效用户 ID 相同
- `getresgid()` 的规则与 `getresuid()` 类似
- `getresgid()` 和 `getresuid()` 调用具有 0/1 效应，即对 ID 的修改请求，要么全成功，要么全失败

# 获取和修改文件系统 ID

上述所有修改进程有效用户 ID 或组 ID 的系统调用总是会修改相应的文件系统 ID，要想独立于有效 ID 而修改文件系统ID：

```
#include <sys/fsuid.h>

int setfsuid(uid_t fsuid);
int setfsgid(uid_t fsgid);
```





















 





