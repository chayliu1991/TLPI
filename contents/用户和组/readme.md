# 密码文件 `/etc/passwd`

系统每个用户账号，系统密码文件 `/etc/passwd`  会专列一行进行描述，每行都包含 7 个字段，之间用冒号处理，如下所示：

```
root:x:0:0:root:/root:/bin/bash
```

- 登录名 ： 登录系统时，用户所必须输入的唯一名称，也称为用户名

- 经过加密的密码 ：经过加密处理的密码，长度为 13 个字符：

  - 当字符串长度超过 13 个字符时，将禁止此账户登录，原因是此类字符串不能代表一个经过加密的有效密码
  - 如果启用了 shadow 密码(常规做法)，系统将不会解析该字段，此时该字段通常显示 `x` 也可以是任何其他的非空字符串，经过加密处理的密码将会存储到 shadow 中
  - 如果 `/etc/passwd` 中密码字段为空，该账户登录时无需密码(即便启用了 shadow 密码)

- 用户 ID(UID) ：用户的数值类型 ID，如果该字段是0，那么相应的账户即具有特权级权限，这种账号一般只有登录名为 root 的账户
  -  允许但不常见同一用户 ID 拥有多条记录，从而使得同一用户 ID 拥有多个登录名，这样，这样多个用户就能以不同的密码访问相同的资源
- 组ID(GID) ：用户属组中首选属组的数值类型 ID
- 注释 ： 该字段存放关于用户的描述性文字
- 主目录 ： 用户登录后所处的初始路径，会以该字段内容来设置 HOME 环境变量
- 登录 shell ：一旦用户登录，便交由该程序控制，如果该字段为空，那么登录 shell 默认为 `/bin/sh`，会以该字段的值来设置 SHELL 环境变量

单机系统中，所有密码信息都存储于 `/etc/passwd` 中，然而，如果使用了 NIS(网络信息系统) 或 LDAP(轻型目录访问协议)在网络环境中分发密码，那么部分密码信息可能由远端系统保存。

# shadow 密码文件 `/etc/shadow`

shadow 密码文件 `/etc/shadow` 的理念是：用户所有的非敏感信息存放于人人可读的密码文件中，而经过加密处理的密码则由 shadow 密码文件单独维护，仅供特权进程读取。

```
root:$6$vem2prCI$x46mC2s/10xA/U3V92SWzj0XiNvLdjTFesf2D.lq9v/b6o4igkLkttfUzEp8KngJoeJTyF.AcejXA5KwVe.RE.:18649:0:99999:7:::
```

- 登录名 ：用来匹配密码文件中相应的记录
- 经过加密的密码
- 其他若干与安全相关的字段

# 组文件 `/etc/group`

对用户进行编组的目的是：控制文件和其他系统资源的访问。

用户所属各组信息的定义由两部分组成：

- 密码文件中相应用户记录的组 ID 字段
- 组文件列出的用户所数组

系统中每个组在组文件 `/etc/group` 中都对应着一条记录

```
root:x:0:
daemon:x:1:
bin:x:2:
```

每条记录 4 个字段，之间用 `:` 隔开：

- 组名 ： 组的名称
- 经过加密处理的密码 ：组密码属于非强制性的
- 组 ID(GID) ： 该组的数值型 ID，对应组 ID 号为0只定义一个名为 root 的组
- 用户列表 ：属于该组的用户名列表，用 `;` 分隔

# 获取用户和组的信息

## 从密码文件获取记录

```
#include <sys/types.h>
#include <pwd.h>

struct passwd *getpwnam(const char *name);
struct passwd *getpwuid(uid_t uid);
```

- `getpwnam()` 提供一个登录名，返回一个指针，指向如下类型的结构体：

``` 
struct passwd {
    char   *pw_name;       /* username */
    char   *pw_passwd;     /* user password */
    uid_t   pw_uid;        /* user ID */
    gid_t   pw_gid;        /* group ID */
    char   *pw_gecos;      /* user information */
    char   *pw_dir;        /* home directory */
    char   *pw_shell;      /* shell program */
};
```

- 要确定是否启用了 shadow 密码，最简单的方法是成功调用 `getpwnam()` 后，紧接着调用 `getspname()`  并观察是否能为同一用户返回一条 shadow 密码记录
- `getpwuid()` 返回的结果与 `getpwnam()` 一致，但是要提供 uid 类型的值作为查询条件
- `getpwuid()` 和 `getpwnam()` 都返回一个指针，指向一个静态分配的结构，故而是不可重入的，它们的可重入版本是 `getpwnam_r()` 和 `getpduid_r()`
- 如果在 passwd 文件中没有发现匹配记录，那么 `getpwuid()` 和 `getpwnam()`  将返回 `NULL`，并且不会改变 `errno`，但是不少 UNIX 实现未遵守 SUSv3 规范，会将 `errno`  设置为非 0 值

## 从组文件获取记录

```
#include <grp.h>

struct group *getgrnam(const char *name);
struct group *getgrgid(gid_t gid);
```

- `getgrnam()` 和 `getgrgid()` 分别通过组名和组 ID 来查找数组的信息，两个函数都会返回一个指针，指向如下类型的结构：

```
struct group {
    char   *gr_name;        /* group name */
    char   *gr_passwd;      /* group password */
    gid_t   gr_gid;         /* group ID */
    char  **gr_mem;         /* NULL-terminated array of pointers
    to names of group members */
};
```

- `getgrnam()` 和 `getgrgid()`  返回的指针也是指向静态分配的结构，因而也是不可重入的，可重入版本分别是 `getgrnam_r()` 和 `getgrgid_r()` 
- 如果没有在 group 文件中发现匹配的记录，其行为与上述的 `getpwuid()` 和 `getpwnam()`  一致

## 扫描密码文件和组文件中所有记录

```
#include <sys/types.h>
#include <pwd.h>

struct passwd *getpwent(void);
void setpwent(void);
void endpwent(void);
```

- `getpwent()` 能够从密码文件中逐条返回记录，当不再有记录或者出错时，该函数返回 `NULL`
- `getpwent()` 调用会自动打开密码文件，密码文件处理完毕时，可以调用 `endpwent()` 将其关闭

```
struct passswd* pwd;
while((pwd = getpwen()) != NULL)
	//@ do sth
endpwent(); //如果在后续调用中 getpwent() 会再次打开密码文件进行扫描，则 endpwent() 是必不可少的
```

- 如果在该文件的处理中途，还可以使用 `setpwent()` 函数返回文件的起始处

## 从 shadow 密码文件中获取记录

下来函数将从 shadow 密码文件中 获取个别记录，以及扫描该文件中的所有记录。

```
#include <shadow.h>

struct spwd *getspnam(const char *name);
struct spwd *getspent(void);

void setspent(void);
void endspent(void);
```

`struct spwd` ：

```
struct spwd {
        char *sp_namp;     /* Login name */
        char *sp_pwdp;     /* Encrypted password */
        long  sp_lstchg;   /* Date of last change
        (measured in days since 1970-01-01 00:00:00 +0000 (UTC)) */
        long  sp_min;      /* Min # of days between changes */
        long  sp_max;      /* Max # of days between changes */
        long  sp_warn;     /* # of days before password expires
        to warn user to change it */
        long  sp_inact;    /* # of days after password expires
        until account is disabled */
        long  sp_expire;   /* Date when account expires
        (measured in days since
        1970-01-01 00:00:00 +0000 (UTC)) */
        unsigned long sp_flag;  /* Reserved */
};
```

# 密码加密和用户认证

UNIX 采用单向加密的方法算法对密码进行加密，这意味着有密码的加密形式将无法还原出原始密码，因此，验证候选密码的唯一方法是使用同一算法对其进行加密，并将加密的结果与存储于 `/etc/shadow`  中的密码进行匹配，加密算法封装于 `crypt()`  函数中。

```
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>

char *crypt(const char *key, const char *salt);
```

- `key` 是一个长达 8 个字符的密码，`salt` 参数指向一个两个字符的字符串，范围是 `[a-zA-Z0-9,]`用来扰动 DES 算法
- 返回一个指针，指向长度为 13 个字符的字符串，该字符串为静态分配而成的，内容就是经过加密处理的密码
- 编译程序时需要开启  `-lcrypt` 选项

```
#include <unistd.h>

char *getpass(const char *prompt);
```

- `getpass()` 获取用户密码，该函数会首先屏蔽回显功能，并停止对终端特殊字符的处理(比如中断字符，一般为 Control+C)，然后打印出 prompt 指向的字符串，读取一行输入，并返回以 `NULL`  结尾的输入字符串作为函数的返回结果
- 返回的字符串是静态分配的，并在返回结果之前将终端设置还原















  



  