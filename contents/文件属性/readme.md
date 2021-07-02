# 获取文件信息

```
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int stat(const char *pathname, struct stat *buf);
int fstat(int fd, struct stat *buf);
int lstat(const char *pathname, struct stat *buf);
```

- `stat()` 返回所命名文件的相关信息
- `lstat()` 与 `stat()` 类似，区别在于如果文件属于符号链接，那么所返回的信息针对的是符合链接自身，而非符号链接所指向的文件
- `fstat()` 返回由某个打开文件描述符所指代的文件的相关信息
- `stat()` 和 `lstat()` 无需对其所操作的文件本身拥有任何权限，但针对指定的 `pathname` 的父目录要有执行权限，而 `fstat()` 只要文件描述符有效，总是成功
- `struct stat`：

```
struct stat {
    dev_t     st_dev;         /* ID of device containing file */
    ino_t     st_ino;         /* inode number */
    mode_t    st_mode;        /* protection */
    nlink_t   st_nlink;       /* number of hard links */
    uid_t     st_uid;         /* user ID of owner */
    gid_t     st_gid;         /* group ID of owner */
    dev_t     st_rdev;        /* device ID (if special file) */
    off_t     st_size;        /* total size, in bytes */
    blksize_t st_blksize;     /* blocksize for filesystem I/O */
    blkcnt_t  st_blocks;      /* number of 512B blocks allocated */

    /* Since Linux 2.6, the kernel supports nanosecond
    precision for the following timestamp fields.
    For the details before Linux 2.6, see NOTES. */

    struct timespec st_atim;  /* time of last access */
    struct timespec st_mtim;  /* time of last modification */
    struct timespec st_ctim;  /* time of last status change */

    #define st_atime st_atim.tv_sec      /* Backward compatibility */
    #define st_mtime st_mtim.tv_sec
    #define st_ctime st_ctim.tv_sec
};
```

## 设备 ID 和 i 节点号

`st_dev` 标识文件所驻留的设备。`st_ino` 包含了文件的 `i` 节点号。利用这两者可在所有文件系统中唯一标识某个文件。

`dev_t` 类型记录了设备的主、辅 ID，使用 `major()` 和 `minor()` 可分别提取。

针对设备的 `i` 节点，`st_rdev` 字段则包含了设备的主、辅 ID。

## 文件所有权

`st_uid` 和 `st_gid` 分别标识文件的属主(用户 ID)和属组(组 ID)。

## 链接数

`st_nlink` 包含了指向文件的硬链接数。

## 文件类型和权限

`st_mode` 包含位掩码，具有标识文件类型和文件权限的双重作用：

![](./img/st_mode.png)

与 `S_IFMT`  相与 `&` 可从该字段中析取文件类型。

```
if((statbuf.st_mode & S_IFMT) == S_IFREG)
	printf("regular file\n");

//@ 可以简写为
if(S_ISREG(statbuf.st_mode))
	printf("regular file\n");
```

![](./img/file_type.png)

- 想要从 `<sys/stat.h>` 中获得 `S_IFSOCK` 和  `S_ISSOCK` 的定义，必须定义 `_BSD_SOURCE`，或是将 `_XOPEN_SOURCE` 定义不小于 500 的值

## 文件大小、已分配块以及最优 IO 块大小

`st_size` ：

- 对于常规文件， 表示文件的字节数
- 对于符合链接，则表示链接所指路径名的长度，以字节位单位
- 对于共享内存对象，该字段则表示对象的大小

`st_blocks` ：

- 表示分配给文件的磁盘块的总块数，块大小为 512 字节，其中包括了为指针所分配的空间。现代的 UNIX 文件系统则使用更大尺寸的逻辑块，例如：1024,2048,4096 字节，其取值总是 2,4,8 的倍数
- 如果文件中内含空洞，该值将小于从相应文件字节数字段 `st_size` 的值
- `du -k file` 可以获取这个值，单位是 KB

`st_blksize`：

- 文件进行 IO 操作时的最优块大小，单位是字节，如果所采用的块大小小于这个值，则视为低效
- 一般 `st_blksize` 是 4096

## 文件时间戳

`st_atime` ： 文件上次访问时间

`st_mtime` ： 文件上次修改时间

`st_ctime` ： 文件状态发生改变的上次时间





















