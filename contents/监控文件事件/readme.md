# 概述

- 使用 `inotify_init()` 创建 `inotify` 实例，该调用返回的文件描述符用于在后续操作中指代该实例
- 使用  `inotify_add_watch()` 向 `inotify` 实例的监控列表添加条目
- 针对 `inotify` 文件描述符执行 `read()` 操作来获取事件
- 程序再结束监控时会关闭 `inotify` 文件描述符，这将自动清除与 `inotify` 实例相关的所有监控项

# inotify_init API

```
#include <sys/inotify.h>

int inotify_init(void);
int inotify_init1(int flags);
```

- 返回一个文件描述符，用来指代 `inotify` 实例
- `inotify_init1`  增加了 `flags` 参数，可以取值：
  - `IN_CLOEXEC` ：使内核针对新文件描述符激活  `close-on-exec`
  - `IN_NONBLOCK` ：使内核激活底层打开文件描述符的 `O_NONBLOCK` 标志

```
#include <sys/inotify.h>

int inotify_add_watch(int fd, const char *pathname, uint32_t mask);
```

- `inotify_add_watch()` 既可以新加监控项，也可以修改现有监控项
- `pathname` 标识欲创建或修改的监控项所对应的文件

![](./img/inotify_kernel_ds.png)

- `mask` 是一位掩码，针对 `pathname` 定义了欲监控的事件
- 如果先前未将 `pathname` 加入到 fd 的监控列表，那么 `inoify_add_watch()` 会再列表中创建一个新的监控项，并返回一新的、非负监控描述符，用来在后续操作中指代此监控项
- 如果先前已将 `pathname` 加入到 fd 的监控列表，，那么 `inoify_add_watch()` 修改现有 `pathname` 监控项的掩码，并返回其监控描述符

```
#include <sys/inotify.h>

int inotify_rm_watch(int fd, int wd);
```

- `inotify_rm_watch()` 会从文件描述符 `fd` 所指代的 `inotify` 实例中删除 `wd` 定义的监控项
- 删除监控项会为该监控描述符生成 `IN_IGNORED` 事件







