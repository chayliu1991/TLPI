# 是否需要一个 Set-User-ID 或 Set-Group-ID 程序

有关编写 `Set-User-ID` 和 `Set-Group-ID` 程序的最佳建议中的一条就是尽量避免编写这种程序。在执行一个任务时如果存在无需赋给程序权限的方法，那么一般来讲应该采用这种方法，因为这样就能消除发生安全性问题的可能。

有时候可以将需要权限才能完成的功能拆分到一个只执行单个任务的程序中，然后在需要的时候在子进程中执行这个程序。

即使有时候需要 `Set-User-ID` 或 `Set-Group-ID`  权限，对于一个 `Set-User-ID` 程序来讲也并不总是需要赋给进程 `root` 身份，如果赋给进程其他一些身份已经足够，那么就应该采用这种方法。

# 以最小权限操作

程序应该总是使用完成当前所执行的任务所需的最小权限来操作，saved 的 `Set-User-ID` 工具就是为此设计的。

## 按需拥有权限

在 `Set-User-ID` 程序中可以临时删除并在之后重新获取权限：

```
uid_t orig_euid;

orig_euid = geteuid();
if(seteuid(geteuid()) == -1)  //@ 使调用进程有效用户 ID 变成真实 ID
	errExit("seteuid()");

//@ do unprivileged work

if(seteuid(orig_euid) == -1) //@ 将有效用户 ID 还原成 save set-user-id 程序中保存的值
	errExit("seteuid()");
```

最安全都额做法是在程序启动的时候立即删除权限，然后在后面需要的时候临时重新获得这些权限，如果在某个特定时刻之后永远不会再次请求权限时，那么程序应该删除这些权限。

### 在无需使用权限时永久地删除权限

如果 `Set-User-ID` 或者 `Set-Group-ID` 程序完成了所有需要权限的任务，那么它应该永久地删除这些权限以消除任何由于程序中包含 bug 或其他意料之外的行为而可能引起的安全风险。永久删除权限是通过将所有进程用户(组) ID 重置为真实(组) ID 来完成的。

对于一个当前有效用户 ID 为 0 的 `Set-User-ID-root` 程序来讲，可以使用下面的代码来重置所有的用户 ID：

```
if(setuid(geteuid()) == -1)
	errExit("setuid()");
```

当调用进程的有效用户 ID 非 0，上面的代码不会重置 `saved set-user-ID`，此时 `seteuid()` 只会修改有效用户 ID

在 `set-group-ID` 程序中永久地删除一个特权组 ID 同样必须要使用 `setregid()` 或 `setresgid()` 系统调用，因为当程序的有效用户 ID 不为 0 时，`setgid()` 只会修改调用进程的有效组 ID。

### 修改进程身份信息的注意事项

- 一些修改进程身份信息的系统调用在不同的系统上的语义是不相同的，此外，此类系统调用中的一些在调用者是特权进程时与非特权进程时表现出来的语义也是不同的
- 在 Linux 上，即使调用者的有效用户 ID 为 0，修改身份信息的系统调用在程序显式地操作其能力时也可能表现出意料之外的行为

由于以上两个可能性，强烈建议不仅需要检查一个修改身份信息的系统调用的成功与否，还需要验证修改行为是否如期的那样。

一些身份信息的变更只能由有效用户 ID 为 0 的进程来完成，因此在修改多个 ID 时：辅助组 ID，组ID，和用户 ID；先删除特权 ID，最后再删除特权有效用户 ID。相应地，在提升特权 ID 时应该先提升特权有效用户 ID。

# 小心执行程序

当一个特权程序通过 `exec()` 直接或者通过 `system()`，`popen()` 以及类似的库函数间接地执行另一个程序时要小心处理。

## 在执行另一个程序之前永久地删除权限

如果一个 `set-user-ID` 或者 `set-group-ID` 程序执行了另外一个程序，那么就应该确保所有的进程用户(组)ID 被重置为真实用户(组)ID，这样新程序在启动时就不会拥有权限。并且无法重新请求这些权限。完成这一任务的一种方式是在执行 `exec()` 之前重置所有的 ID。

在调用 `exec()` 之前调用 `setuid(getuid())` 能够取得同样的结果。

假设现有进程：

```
real = 1000 effective = 200 saved = 200
```

`setuid(getuid())`  执行后：

```
real = 1000 effective = 1000 saved = 200
```

当进程执行一个非特权程序时，进程的有效用户 ID 会被复制到 `saved set-user-ID` ，从而导致进程的用户 ID 变为：

```
real = 1000 effective = 1000 saved = 1000
```

## 避免执行一个拥有权限的 shell(或者其他解释器)

运行于用户控制之下的特权程序永远都不应该通过 `system()`， `popen()`， `execlp()`， `execvp()`，或者其他类似函数直接或间接地执行 shell。如果必须要执行 shell，那么就需要确保在执行之前永久地删除权限。

Linux 与其他一些 UNIX 实现一样，在执行脚本时会毫无征兆地忽略  `set-user-ID` 和 `set-group-ID` 权限位，即使在允许  `set-user-ID` 和 `set-group-ID` 脚本的实现上也应该避免使用它们。

## 在 `exec()` 之前关闭所有用不到的文件描述符

在 `exec()` 调用之间文件描述符会保持在打开状态，特权进程可能会打开普通进程无法访问的文件，这种打开的文件描述符表示一种特权资源。在调用 `exec()` 之前应该关闭这种文件描述符，这样被执行的程序就无法访问相关的文件了。

完成这个任务可以通过显式关闭文件描述符，也可以设置程序的 `close-on-exec` 标记。

# 避免暴露敏感信息

当一个程序读取密码或者其他敏感信息时，应该在执行完所需的处理之后立即从内存中删除这些信息。在内存中保留这些信息是一种安全隐患：

- 包含这些数据的虚拟内存页面可能会被换出，这样交换区域中的数据可能会被一个特权进程读取
- 如果进程收到了一个可能导致它产生 core dump 的信号，那么就有可能会从该文件中获取这类信息

# 确定进程的边界

## 考虑使用能力

Linux 能力模型将传统的 all-or-nothing UNIX 权限模型划分为一个个被称为能力的单元。一个进程能够独立地启用或者禁用单个能力。通过只启用进程所需的能力使得程序能够在不拥有完整的 root 权限的情况下运行，这样可以降低程序发生安全问题的可能性。

使用能力和 securebits 标记可以创建只拥有有限的一组权限但无需属于 root 的进程。

## 考虑使用一个 chroot 监牢

建立一个 chroot 监牢来限制程序能够访问的一组目录和文件，但是 chroot 监牢不足以限制一个 `set-user-ID-root` 程序。

# 小心信号和竞争条件

用户可以向他启动的 `set-user-ID` 程序发送信号，其发送时间和发送频率也是任意的。当信号在程序执行过程中的任意时刻发送时需要考虑可能出现的竞争条件。在程序中合适的地方应该捕获、阻塞或忽略信号以防止可能存在的安全性问题，另外信号处理器的设计应该尽可能简单以降低无意中创建竞争条件的风险。

# 执行文件操作和文件 IO 的缺陷

如果一个特权进程需要创建一个文件，那么必须要小心处理这个文件的所有权和权限以确保文件不存在被恶意操作攻击的风险。应该遵循的准则：

- 需要将进程的 `umask` 设置为一个能确保进程永远创建公共可写的文件的值，否则恶意用户就能修改这些文件
- 由于文件的所有权是根据创建进程的有效用户 ID 来确定的，因此可能需要使用 `seteuid()` 或者 `setreuid()` 来临时修改进程的身份信息以确保新创建的文件不会属于错误的用户，类似的规则也适用于 `set-group-ID`  程序
- 如果一个 `set-user-ID-root` 程序必须要创建一个一开始由其自己拥有但最终由另一个用户拥有的文件，那么所创建的文件在一开始应该不对其他用户开放写权限，可以通过向 `open()` 传入合适的 `mode` 参数或者在调用 `open()` 之前设置进程的 `umask` 完成，之后程序可以使用 `fchown()` 和 `fchmod()` 来修改文件的所有权和文件的权限
- 在打开的文件描述符上检查文件的特性（在 `open()` 调用之后再调用 `fstat()`)，而不是检查与一个路径名相关联的特性后再打开文件(如在 `stat()` 之后再调用 `open()`)，后一种方法存在使用时间和检查时间的问题
- 如果一个程序必须要确保它自己是文件的创建者，那么在调用 `open()` 时应该使用 `O_EXCL` 标记
- 特权进程应该避免创建或依赖像 `/tmp` 这样的公共可写目录，因为这样程序就容易受到那些试图创建文件名与特权程序预期一致的非授权文件的恶意攻击，一个必须要在某个公共可写的目录中创建文件的程序应该至少要使用诸如 `mkstemp()` 之类的函数确保这个文件的文件名不会不可预测

# 不要完全相信输入和环境

特权进程应该避免完全信任输入和它们所运行的环境。

## 不要信任环境列表

`set-user-ID` 和 `set-group-ID`  程序不应该假设环境变量的值是可靠的，特别是 `PATH` 和 `IFS` 两个变量。

`PATH` 确定 shell(以及 `system()` 和 `popen()`) 以及 `execlp()` 和 `execvp()` 在何处搜索程序，恶意用户可以改变 `PATH` 的值以欺骗 `set-user-ID` 程序使它在使用其中一个函数时会导致拥有权限的情况下执行任意一个程序。在使用这些函数时应该将 `PATH` 值设置为一个可信的目录列表，更好的做法是在执行程序时指定绝对路径名。

`IFS`  指定了 shell 解释器用来分隔命令行中的单词的分隔符，应该将这个变量设置为任意一个空字符串，表示 shell 只会把空字符当成单词分隔符。

在某些情况下，特别是在执行其他程序或调用可能受到环境变量影响的库时，最安全的方式是删除整个环境列表，然后使用已知的安全值来还原所选中的环境变量。

## 防御性地处理不可信用户的输入

特权程序应该在根据来自不可信源的输入采取动作之前小心地验证这些输入，包括数字范围，字符串长度等等。

## 避免对进程的运行时环境进行可靠性假设

`set-user-ID` 程序应该避免假设其初始的运行环境是可靠的，如标准输入、标准输出或错误可能会被关闭。这样打开一个文件时可能会无意中复用描述符1，从而导致程序认为正在往标准输出中输出数据。

# 小心缓冲区溢出

当输入值或复制的字符串超出分配的缓冲区空间时就需要小心缓冲区溢出。

永运不要使用 `gets()`，在使用诸如 `scanf()`， `sprintf()`，`strcpy()`以及 `strcat()` 时需要注意，可以使用安全的版本：`snprintf()`，`strncpy()`，`strncat()`，但是使用这些函数时也需要注意：

- 对于其中的大多数函数来讲，如果达到了指定的最大值，那么源字符串的截断部分会被放到目标缓冲区中，因此需要检查是否发生了截断
- 使用 `strncpy()`  对性能会有影响，如在 `strncpy(s1,s2,n)` 调用中如果 `s2` 的长度小于 `n`，那么补全  `null` 字节会被写入到 `s1` 以确保写入 `n` 个字节
- 如果传入 `strncpy()` 的最大大小不足以容纳结尾的 `null` 字符，那么目标字符串就会不以 `null` 结尾

恶意用户可以通过诸如缓冲区溢出也成为栈粉碎之类的技术将精心编写的字节放入一个栈栈中以强制特权程序执行任意代码。

# 小心拒绝服务攻击

随着基于 Internet 服务的增长，系统相应收到远程拒绝服务 DOS 的攻击的可能性也在增长。这些攻击通过向服务器发送能导致其崩溃的错误数据或使用虚假请求服务器增加过量的负载使得系统无法向合法用户提供正常的服务。

处理错误的请求比较直观：严格执行输入检查。

超负荷攻击比较难以处理：由于服务器无法控制远程客户端的行为以及它们提交请求的速率，因此这样的攻击几乎无法防止。

应对超负荷攻击的需要注意：

- 服务器应该执行负载控制，当负载超过预设值之后就丢弃请求，这可能会导致丢弃合法的请求，但能够防止服务器和主机机器的负载过大
- 服务器应该为与客户端的通信设置超时时间，这样如果客户端不响应，那么服务器也不会永远地等待客户端
- 在发生超负荷时，服务器应该记录下合适的信息以便系统管理员能得知这个问题
- 服务器程序遇到预期之外的负载时不应该崩溃
- 设计使用的数据结构应该能够避免算法复杂度攻击，如二叉树应该是平衡的，并且在常规负载下应该能提供可接受的性能

# 检查返回状态和安全地处理失败的情况

特权程序应该总是检查系统调用和库函数调用是否成功以及它们是否返回了预期的值。

各种各样的系统调用都可能会失败，即使程序以 `root` 身份运行。

即使系统调用成功了也有必要检查结果，如在需要的时候，特权程序应该检查成功的 `open()` 调用没有返回的三个标准文件描述符0,1,2 中的某个。

如果特权程序碰到了未知情形，那么恰当的处理方式通常是终止执行或者如果是服务器的话就丢弃客户端的请求。 









































 











