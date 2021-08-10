# 整体概览

传统型终端和终端模拟器都需要同终端驱动程序相关联，由驱动程序负责处理设备上的输入和输出。

当执行输入时，驱动程序可以工作在以下两种模式下：

- 规范模式：在这种模式下，终端的输入是按行来处理的，而且可进行行编辑操作。每一行都由换行符来结束，当用户按下回车键时可产生换行符。在终端上执行的 `read()` 调用只会在一行输入完成之后才会返回，且最多只会返回一行。这是默认的输入模式
- 非规范模式：终端输入不会被装配成行，像 `vi`，`more`，`less` 这样的程序会将终端置于非规范模式，这样不需要用户按下回车键它们就能读取到单个字符

终端驱动程序也能对一系列的特殊字符做解释，比如终端字符 `Ctrl+C` 以及文件结尾符 `Ctrl+D`。当有信号为前台进程组产生时，又或者是程序在从终端读取时出现某种类型的输入条件，此时就可能会出现这样的解释操作。将终端置于非规范模式下的程序通常也会禁止处理某些或者所有这些特殊字符。

终端驱动程序会对两个队列做操作：

- 一个用于从终端设备将输入字符传送到读取进程上
- 另一个用于将输出字符从进程传送到终端上

如果开启了终端回显功能，那么终端驱动程序会自动将任意的输入字符插入到输出队列的尾部，这样输入字符也会成为终端的输出：

![](./img/terminate_in_out.png)

# 获取和修改终端属性

函数 `tcgetattr()` 和 `tcsetattr()` 可以用来获取和修改终端的属性：

```
#include <termios.h>
#include <unistd.h>

int tcgetattr(int fd, struct termios *termios_p);
int tcsetattr(int fd, int optional_actions,const struct termios *termios_p);
```

- `fd` 是指向终端的文件描述符，如果 `fd` 不指向终端，调用这些函数就会失败，伴随错误码 `ENOTTY`
- `termios_p` 是一个指向 `struct termios` 的指针，用来记录终端的各项属性：

```
struct termios
{
    unsigned short c_iflag; 	/* Input flags */
    unsigned short c_oflag; 	/* Output flags */
    unsigned short c_cflag; 	/* control flags */
    unsigned short c_lflag; 	/* Local modes */
    unsigned char c_line; 		/* Line discipline (nonstandard) */
    unsigned char c_cc[NCCS];	 /* Terminal special characters */
    speed_t  c_ispeed;			/* Input speed (nonstandard;unused)*/
    speed_t c_ospeed;			/* Output speed (nonstandard;unused)*/
};
```

- `termios` 前 4 个字段都是位掩码，包含有可控制终端驱动程序各方面操作的标志：
  - `c_iflag` ： 包含控制终端输入的标志
  - `c_oflag` ： 包含控制终端输出的标志
  - `c_cflag` ： 包含与控制终端想速的硬件控制相关的标志
  - `c_lflag` ： 包含控制终端输入的用户界面的标志
- `c_line` 字段指定了终端的行规程，为了达到对终端模拟器编程的目的，行规程将一直设为 `N_TTY`，也就是所谓的新规程
- `c_cc` 包含着终端的特殊字符，以及用来控制非规范模式下输入操作的相关字段
- `c_ispeed` 和 `c_ospeed` 字段在 Linux 上没有使用到
- `optional_actions` 用来确定何时修改将生效，该参数可以被指定为下列的值：
  - `TCSANOW` ：修改立刻得到生效
  - `TCSADRAIN`：当所有当前处于排队中的输出已经传送到终端之后，修改得到生效。通常，该标志应该在修改影响终端的输出时才会指定，这样就不会影响到已经处于排队中，但是还没有显示出来的输出数据
  - `TCSAFLUSH` ：该标志的产生的效果同 `TCSADRAIN`，但是除此之外，当标志生效时那些仍然等待处理的输入数据都会被丢弃

通常修改终端属性的方法是调用 `tcgetadttr()` 来获取一个包含有当前设定的 `termios` 结构体，然后调用 `tcsetattr()` 将更新后的结构体传回给驱动程序：

```
struct termios tp;

if(tcpgetattr(STDIN_FILENO, &tp) == -1){
	errExit("tcpgetattr()");
}

tp.c_lflag &= ~ECHO;

if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1){
	errExit("tcsetattr()");
}
```

如果任何一个对终端属性的修改请求可以执行的话，函数 `tcsetattr()` 将返回成功；它只会在没有任何修改请求能执行时才会返回失败。这意味着当我们修改多个属性时，有时可能有必要再调用一次 `tcgetattr()` 来获取新的终端属性，并同之前的修改请求做对比。





































