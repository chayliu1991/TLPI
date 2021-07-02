# 动态加载库

`dlopen API` 使得程序能够在运行时打开一个共享库，根据名字在库中搜索一个函数，然后调用这个函数，称为动态加载库，要在 Linux 上使用 `dlopen API`  构建程序，必须指定 `-ldl` 选项以便与 `libdl` 库链接起来。

## 打开共享库

```
#include <dlfcn.h>

void *dlopen(const char *filename, int flags);
```

- `filename` 中如果包含了 `/`，那么 `dlopen()` 会将其解释成一个绝对或者相对路径名，否则动态链接器将会使用之前的规则搜索共享库

- 成功时返回一个句柄，在后续对 `dlopen API` 中的函数调用可以使用该句柄来引用库，如果发生了错误(例如，无法找到库)，那么 `dlopen()` 返回 `NULL`

- 如果 `filename` 指定的共享库依赖于其他共享库，`dlopen()` 会自动加载这些库，并递归进行，形成这个库的依赖树

- 多次调用  `dlopen()` 将返回同一句柄，但是引用计数会增加，每次调用 `dclose()` 都会减小引用计数，只有当引用计数为 0 时，`dclose()` 才会从内存中删除这个库

- 将 `filename` 设置为 `NULL` 时，`dlopen()` 会返回主程序的句柄，后续对 `dlsym()` 的调用中使用这个句柄会使得：

  - 首先在主程序中搜索符号
  - 然后再程序启动时加载的共享库中进行搜索
  - 最后在所有使用了 `RTLD_GLOBAL` 标记的动态加载库中搜索

- `flags` 是一个位掩码：

  - `FTLD_LAZY` ：只有当代码被执行时才解析库中未定义的函数符号，如果某个特定符号的代码没有被执行到，那么将永远不会解析该符号，延迟解析只适用于函数引用，对变量的引用会被立即解析
  - `RTLD_NOW` ：在 `dlopen()` 结束之前立即加载库中所有未定义的符号，不管是否需要用到这些符号，这种做法的结果是打开库变得更慢，但能够立即检测到潜在的未定义函数符号错误
  - 设置环境变量 `LD_BIND_NOW` 为一个非空字符串能够强制动态链接器在加载可执行文件的动态依赖列表中的共享库时立即解析所有符号，类似 `RTLD_NOW`，并且会覆盖 `RTLD_LAZY` 标记的效果
  - `RTLD_GLOBAL` ： 这个库及其依赖树中的符号在解析由这个进程加载的其他库中的引用和通过 `dlsym()` 查找时可用
  - `RTLLD_LOCAL` ：与 `RTLD_GLOBAL`  相反，如果不指定任何常量，那么就取得这个默认值，它规定在解析后续加载的库中的引用时，这个库及其依赖树中的符号不可用
  - `RTLD_NODELETE` ：`dlclose()`  调用不要卸载库，即使其引用计数变为 0，这意味着在后面重新通过 `dlopen()`  加载库时不会重新初始化库中的静态变量，`gcc -Wl,-znodelete` 选项能够达到类似效果
  - `RTLD_NOLOAD` ：不加载库：
    - 使用这个标记来检查某个特定的库是否已经加载到了进程的地址空间中，如果已经加载了，那么 `dlopen()` 返回库的句柄，如果没有，返回 `NULL`
    - 使用这个标记来提升已加载库的标记，如在堆对之前使用 `RTLD_LOCAL` 打开的库调用 `dlopen()` 可以在 `flags` 参数中指定 `RTLD_NOLOAD | RTLD_GLOBAL`
  - `RTLD_DEEPBIND` ：在解析这个库中的符号引用时先搜索库中的定义，然后再搜索已加载的库中的定义，使得一个库能够实现自包含，即优先使用自己的符号定义，而不是在已加载的其他库中定义的全局符号，`-Bsymbolic` 链接器选项能够具有类似效果

  

## 错误诊断

  ```
  #include <dlfcn.h>
  
  char *dlerror(void);
  ```

  - `dlerror()` 返回 `dlopen()` 返回的错误，如果没有错误，返回 `NULL`

## 获取符号的地址

```
#include <dlfcn.h>

void *dlsym(void *handle, const char *symbol);

#define _GNU_SOURCE
void *dlvsym(void *handle, char *symbol, char *version);
```

- `dlsym()` 在 `handle` 指向的库以及该库的依赖树中的库中搜索名为 `symbol` 的符号，找到了返回其地址，否则返回 `NULL`
- `handle` 参数通常是 `dlopen()` 返回的库句柄，或者一个伪句柄 
- `dlvsym()` 的功能与 `dlsym()` 类似，但是能够指定版本号
- `dlsym()` 返回 `NULL` 无法判断是否发生了错误，应该在调用之前调用 `dlerror()` 清除错误字符串，再调用 `dlsym()` 如果  `dlerror()` 返回非 `NULL` 则表示发生了错误
- `symbol` 如果是一个变量名称，可以直接赋值给指针，并接引用得到值
- `symbol` 如果是函数名称，则可以使用返回的指针调用函数

### 在` dlsym()` 中使用伪句柄

除了使用 `dlopen()` 调用的返回值，还可以是下列伪句柄：

- `RTLD_DEFAULT` ：从主程序开始查找 `sybmbod`，接着按序从所有已加载的共享库中查找，包括那些使用了 `RTLD_GLOBAL` 标记的 `dlopen` 调用动态加载库，这个标记对应于动态链接器所采用的默认搜索类型
- `RTLD_NEXT` ：在调用 `dlsym()` 之后加载的共享库中搜索 `sybmbol`，这个标记用于需要创建与在其他地方定义的函数同名的包装函数情况

## 关闭共享库

```
#include <dlfcn.h>

int dlclose(void *handle);
```

- `dlclose()` 会减小 `handle` 所引用的库的打开系统计数，如果这个引用计数变为 0，并且其他库也不需要用到该库中的符号，就会卸载这个库
- 系统会在这个库的依赖树中执行同样的操作
- 当进程终止时会隐式地对所有库执行 `dlclose()`

## 获取与加载的符号的相关信息

```
#define _GNU_SOURCE
#include <dlfcn.h>

int dladdr(void *addr, Dl_info *info);
```

- `info` 是一个指向调用者分配的结构的指针，其形式为：

```
typedef struct {
    const char *dli_fname;  /* Pathname of shared object that contains address */
    void       *dli_fbase;  /* Base address at which shared object is loaded */
    const char *dli_sname;  /* Name of symbol whose definition overlaps addr */
    void       *dli_saddr;  /* Exact address of symbol named in dli_sname */
} Dl_info;
```

- `Dl_info` 结构体的前两个字段指定了包含地址 `addr` 的共享库的路径名和运行时基地址，最后两个字段返回地址相关的信息，如果 `addr` 指向共享库中一个符号的确切地址，那么 `dli_saddr` 返回值与传入的 `addr` 值一样

## 在主程序中访问符号

如果使用 `dlopen()` 动态加载一个共享库，然后使用 `dlsym()` 获取共享库中 `x()` 地址，接着调用 `y()`，那么通常会在程序加载的其中一个共享库中搜索 `y()`。

如果需要让 `x()` 调用主程序中的 `y()` 实现，类似于回掉 ，为了达到这个目的就必须使主程序中的符号对动态链接器可用，即在链接程序时使用 `--export-dynamic` 选项。

```
gcc -Wl,--export-dynamic main.c
```

# 控制符号的可见性

在 C 程序中可以使用 `static` 关键词使得一个符号私有于一个源代码模块，从而使得它无法被其他目标文件绑定。这样带来的负面影响是在同一源文件中对该符号的所有引用都会被绑定到该符号的定义上，而不会被关联到其他共享库中的相应定义。

gcc 提供一个特有声明，它执行与 `static` 关键词类似的任务：

```
void __attribute__((visibility("hidden"))) func(void)
{
	//@ code
}
```

`static`  关键词将一个符号的可见性限制在单个源代码文件中，而 `hidden` 特性使得一个符号对构成共享库的所有源代码都可见，但是对库之外的文件不可见。

当动态加载一个共享库是，`dlopen()` 接收的 `RTLD_GLOBAL` 标记可以用来指定这个库中定义的符号应该应用于后续加载的库中的绑定操作，`--export-dynamic` 链接器选项可以用来使主程序的全局符号对动态加载的库可用。

# 链接器版本脚本

版本脚本是一个包含链接器 `ld` 执行指令的文本文件，要使用版本脚本必须指定 `--version-script` 链接器选项：

```
gcc -Wl,--version-script,myscript.map ...
```

## 使用版本脚本控制符号的可见性

假设 `vis_comm.c` 中定义 `vis_comm()`, `vis_f1.c` 中定义 `vis_f1()`, `vis_f2.c` 中定义 `vis_f2()`, `vis_comm()` 由 `vis_f1()` 和 `vis_f2.c` 调用，但是不想让链接库的程序直接使用。

可以使用版本脚本：

```
# vis.map

VER_1{
	global:
		vis_f1;
		vis_f2;
	local:
		*;
};
```

- `VER_1` 是一种版本标签，如果使用版本脚本只是为了控制符号的可见性，那么版本标签是多余的，但老版本的 `ld` 仍然需要使用这个标签，现代版本的 `ld` 允许省略版本标签，如果省略了版本标签，就认为版本节点拥有一个匿名版本标签并且在这个脚本中不能存在其他版本节点
- `global` 标记出对库之外的程序可见的符号列表的起始位置，`local` 标记出对库之外的程序隐藏的符号列表的起始位置

```
gcc -g -c -fPIC -Wall vis_comm.c vis_f1.c vis_f2.c
gcc -g -c -shared -o vis.so vis_common.o vis_f1.o vis_f2.o -Wl,--version-script,vis.map
```

## 符号版本化

符号版本化允许一个共享库提供同一个函数的多个版本，每个程序会使用它与共享库进行静态链接时函数的当前版本。

```
# sv_lib_v1.c

#include <stdio.h>
void xyz(void){printf("v1,xyz\n");}

# sv_v1.map
VER_1{
	global:xyz;
	local:*;
};
```

```
gcc -g -c -fPIC -Wall sv_lib_v1.c
gcc -g -shared -o libsv.so sv_lib_v1.o -Wl,--version-script,sv_v1.map
```

假设现在需要修改 `xyz()` 的定义，但是原来已经使用 `xyz()` 的程序要仍旧使用老版本，必须在库中定义两个版本的  `xyz()` ：

```
# sv_lib_v1.c

#include <stdio.h>

__asm__(".symver xyz_old,xyz@VER_1");
__asm__(".symver xyz_new,xyz@@VER_2"); //@@ 指示应用程序与这个共享库静态链接时应该使用的 xyz() 默认定义

void xyz_old(void){printf("v1,xyz\n");}
void xyz_new(void){printf("v2,xyz\n");}
void pqr(void){printf("v2 pqr\n");}
```

```
# sv_v2.map

VER_1{
	global:xyz;
	local:*;
};

VER_2{
	global:pqr;
}VER_1;
```

- `VER_2` 依赖于 `VER_1` ,版本依赖可以继承 `local` 和 `global` 规范；依赖可以串联起来
- `VER_2` 中 `global` 中指定了 `pqr()`,如果不这样，将从 `VER_1` 继承而来的规范将导致 `pqr()` 对外不可见  

```
gcc -g -c -fPIC -Wall sv_lib_v2.c
gcc -g -shared -o libsv.so sv_lib_v2.o -Wl,--version-script,sv_v2.map
```

# 初始化和终止函数

可以定义一个或多个在共享库被加载和卸载时自动执行的函数。

初始化和终止函数是使用 gcc 的 `constructor` 和 `destructor` 特性来定义，在库被加载时需要执行的所有函数应该定义成下面的形式：

```
void __attribute__((constructor)) some_name_load(void)
{
}
```

卸载函数的形式：

```
void __sttribute__((destructor)) some_name_unload(void)
{
}
```

- `constructor` 和 `destructor`  也能创建主程序的初始化函数和终止函数

## `_init()` 和 `_fini()`

完成共享库的初始化和终止工作的较早的技术是在库中创建两个函数 `_init()` 和 `_fini()` 函数。

如果创建了 `_init()` 和 `_fini()` 函数，构建库时必须指定 `gcc -nostartfiles` 选项以防止链接器加入这些函数的默认实现，如果需要的话，可以施用 `-Wl,-init` 和 `-Wl,-fini` 选项指定函数的名称。

有了 `gcc` 的 `constructor` 和  `destructor` 特性后，不建议使用 `_init()` 和  `_fini()`。

# 预加载共享库

环境变量 `LD_PRELOAD` 包含了在加载其他共享库之前需要加载的共享库名称，共享库名称之间使用空格和冒号分隔。由于首先会加载这些库，因此可执行文件自动会使用这些库中定义的函数，从而覆盖那些动态链接器在其他情况下搜索的同名函数。

出于安全的原因，在 `set-user-ID` 和 `set-setgroup-ID` 程序中将会忽略 `LD_PRELOAD`。

# 监控动态链接器

将环境变量 `LD_DEBUG`  设置为一个或多个标准关键词可以从动态链接器中得到各种跟踪信息：

如果将 `help` 赋给 `LD_DEBUG`，那么动态链接器会输出有关 `LD_DEBUG` 的帮助信息，而指定的命令不会被执行。

```
LD_DEBUG=help date
Valid options for the LD_DEBUG environment variable are:

  libs        display library search paths
  reloc       display relocation processing
  files       display progress for input file
  symbols     display symbol table processing
  bindings    display information about symbol binding
  versions    display version dependencies
  scopes      display scope information
  all         all previous options combined
  statistics  display relocation statistics
  unused      determined unused DSOs
  help        display this help message and exit
```

要将调试信息输出到一个文件中而不是标准输出中，则可以使用 `LD_DEBUG_OUTPUT` 环境变量指定一个文件名。

当请求与跟踪库搜索相关的信息时会产生很多输出：

```
LD_DEBUG=libs date
     29623:     find library=libc.so.6 [0]; searching
     29623:      search cache=/etc/ld.so.cache
     29623:       trying file=/lib/x86_64-linux-gnu/libc.so.6
     29623:
     29623:
     29623:     calling init: /lib/x86_64-linux-gnu/libc.so.6
     29623:
     29623:
     29623:     initialize program: date
     29623:
     29623:
     29623:     transferring control: date
     29623:
```

- `29623` 是跟踪的进程的进程 ID
- 如果需要的话可以给 `LD_DEBUG` 赋多个选项，各个选项之间用逗号分隔(不能出现空格)，
- `LD_DEBUG` 对于由动态链接器隐式加载的库和使用 `dlopen()` 动态加载的库都有效
- 出于安全的原因，在 `set-user-ID` 和 `set-setgroup-ID` 程序中将会忽略 `LD_DEBUG`





 



