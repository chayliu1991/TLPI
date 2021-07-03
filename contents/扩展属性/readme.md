文件的扩展属性(EA)，即以名称-值形式对任意元数据与文件 `i` 节点关联起来的技术。

# 概述

EA 可用于实现访问列表和文件能力。

## EA 命名空间

EA 的命名格式为 `namespace.name`：

- `namespace` 把 EA 从功能上划分为截然不同的几大类

  - `user EA` ： 将在文件权限检查的制约条件下由非特权级进程操控，欲获取 `user EA` 的值，需要有文件的读权限；欲改变 `user EA` 的值，则需要写权限
  - `trusted EA` ：也可由用户进程 “驱使”，要操纵 `trusted EA` 进程必须具有特权
  - `system EA` ：供内核使用，将系统对象与一文件关联
  - `security EA` ：用来存储服务于操作系统安全模块的文件安全标签；将可执行文件与能力关联起来

  

- `name` 则用来在既定命名空间内唯一标识某个 EA

一个 `i` 节点可以拥有多个相关 EA，其所从属的命名空间可以相同，也可以不同。

## 通过 shell 创建并查看 EA

`setfattr` 和 `getfattr` 命令可以用来设置和查看文件的 EA。

# 扩展属性的实现细节

## 对 user 扩展属性的限制

`user EA`  只能施之于文件或目录，之所以将其他文件类型排除在外：

- 对于符号链接，会对所有用户开启所有权限，且不容更改
- 对于设备文件、套接字以及 FIFO，授予用户权限，意在对其针对底层对象所执行的 IO 操作加以控制

## EA 在实现方面的限制

Linux VFS 针对所有文件系统上 EA 的限制：

- EA 名称的长度不能超过 255 个字节
- EA 值得容量为 64 KB

此外，某些文件系统对可与文件挂钩的 EA 数量及其大小还有更严格的限制：

- 在 `ext2`、 `ext3`、 `ext4` 文件系统，与一文件关联的所有 EA 命名和 EA 值得总字节数不超过单个逻辑磁盘块的大小：1024字节，2048 字节，4096字节
- 在 JFS 上，为某一文件所使用的所有 EA 名和 EA 值得总字节数上限为 128 KB

# 操控扩展属性的系统调用

## 创建和修改 EA

```
#include <sys/xattr.h>

int setxattr(const char* pathname,const char* name,const void* value,size_t size,int flags);
int lsetxattr(const char* pathname,const char* name,const void* value,size_t size,int flags);
int fsetxattr(int fd,const char* name,const void* value,size_t size,int flags);
```

- `setxattr()` 通过 `pathname` 标识文件，若文件名为符号链接，则对其解引用
- `lsetxattr()` 通过 `pathname` 标识文件，若文件名为符号链接，不会对其解引用
- `fsetxattr()` 通过 `fd` 标识文件
- `name` 是一个以空字符结尾的字符串，定义了 EA 的名称
- `value` 是一个指向缓冲区的指针，包含了为 EA 定义的新值，参数 `size`  则表明了缓冲区的大小
- 如果 EA 不存在则会创建一个新 EA，如果 EA 已经存在，则替换 EA 的值
- `flags` 参数如果指定为 0，则获得默认行为，或者将其指定为：
  - `XTTR_CREATE` ：若具有给定名称的 EA 已经存在，则失败
  - `XTTR_REPLACE` ：若具有给定名称的 EA 不存在，则失败

## 获取 EA 的值

```
#include <sys/xattr.h>

ssize_t getxattr(const char* pathname,const char* name,void* value,size_t size);
ssize_t lgetxattr(const char* pathname,const char* name,void* value,size_t size);
ssize_t fgetxattr(int fd,const char* name,void* value,size_t size);
```

- `name` 是一个以空字符结尾的字符串，用来标识欲取值的 EA
- 返回的 EA 保存在 `value` 指向的缓冲区中，该缓冲区必须由调用者分配，并用 `size` 指定大小
- 调用成功，返回复制到 `value` 所指向缓冲区中的字节数
- 若文件不包含名为 `name` 的属性，上述调用将会失败，返回错误 `ENODATA`，若 `size`  值过小，上述调用也会失败，返回 `ERANGE`
- 若 `size` 指定为0，将忽略 `value` 值，但系统调用仍将返回 EA 值得大小，可利用这个机制来确定后续系统调用在实际获取 EA 值时所需的  `value` 缓冲区大小，但是不能保证后续在通过系统调用获取 EA 时，上述返回值就足够大

## 删除 EA

```
#include <sys/xattr.h>

int removexattr(const char* pathname,const char* name);
int lremovexattr(const char* pathname,const char* name);
int fremovexattr(int fd,const char* name);
```

- `name`  所包含以空字符结尾的字符串，用于标识要删除的 EA
- 试图删除不存在的 EA，调用将失败，并返回错误  `ENODATA`

## 获取与文件相关联的所有 EA 的名称

```
#include <sys/xattr.h>

ssize_t listxattr(const char* pathname,char* list,size_t size);
ssize_t llistxattr(const char* pathname,char* list,size_t size);
ssize_t flistxattr(cint fd,char* list,size_t size);
```























