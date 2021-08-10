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

# stty 命令

`stty` 命令是以命令行的形式来模拟 `tcgetaddr()` 和 `tcsetaddr()` 的功能，允许我们在 shell 上检视和修改其属性。当我们监视、调试或者取消程序修改的终端属性时，这个工具非常有用。

可以采用如下的命令检视所有终端的当前属性：

```
stty -a
speed 38400 baud; rows 56; columns 206; line = 0;
intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>; eol2 = <undef>; swtch = <undef>; start = ^Q; stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W; lnext = ^V; discard = ^O; min = 1; time = 0;
-parenb -parodd -cmspar cs8 -hupcl -cstopb cread -clocal -crtscts
-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr icrnl ixon -ixoff -iuclc ixany -imaxbel -iutf8
opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
isig icanon iexten echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke -flusho -extproc
```

- 上述输出的第一行显示出了终端的线速（比特每秒）、终端的窗口大小以及以数值形式给出的行规程（0 代表 `N_TTY`，即新行规程）
- 接下来的 3 行显示出了有关各种终端特殊字符的设定。`^C`  表示 `Ctrl+C`，以此类推。字符串 `<undef>` 表示相应的终端特殊字符目前没有定义。`min` 和 `time` 的值与非规范模式下的输入有关
- 剩下的几行显示出了` termios` 结构体中 `c_cflag`、`c_iflag`、`c_oflag` 以及 `c_lflag` 字段中各个标志的设定（按顺序显示）。这里的标志名前带有一个连字符（`-`）的表示目前被禁用，否则表示当前已设定

如果输入命令时不加任何命令行参数，那么 `stty` 只会显示出线速、行规程以及任何其他偏离了正常值的设定：

```
stty
speed 38400 baud; line = 0;
-brkint ixany -imaxbel
```

# 终端特殊字符

![](./img/special_character.png)

## CR

`CR` 是回车符。这个字符会传递给正在读取输入的进程。在默认设定了 `ICRNL` 标志（在输入中）将 `CR` 映射为 `NL`）的规范模式下（设定 `ICANON` 标志），这个字符首先被转换为一个换行符，然后再传递给读取输入的进程。如果设定了 `IGNCR`（忽略 `CR` 标志），那么就在输入上忽略这个字符（此时必须用真正的换行符来作为一行的结束）。输出一个 `CR` 字符将导致终端将光标移到一行的开始处

## DISCARD

`DISCARD` 是丢弃输出字符。尽管这个字符定义在了数组 `c_cc` 中，但实际上在 Linux 上没有任何效果。在一些其他的 Unix 实现中，一旦输入这个字符将导致程序输出被丢弃。这个字符就像一个开关----再输入一次将重新打开输出显示。当程序产生大量输出而我们希望略过其中一些输出时这个功能就非常有用。（在传统的终端上这个功能更加有用，因为此时线速会更加缓慢，而且也不存在什么其他的“终端窗口”。）这个字符不会发送给读取进程

## EOF

`EOF` 是传统模式下的文件结尾字符（通常是 `Ctrl+D`）。在一行的开始处输入这个字符会导致在终端上读取输入的进程检测到文件结尾的情况（即，`read()` 返回 0）。如果不在一行的开始处，而在其他地方输入这个字符，那么该字符会立刻导致 `read()` 完成调用，返回这一行中目前为止读取到的字符数。在这两种情况下，`EOF` 字符本身都不会传递给读取的进程

## EOL 以及 EOL2

`EOL` 和 `EOL2` 是附加的行分隔字符，对于规范模式下的输入，其表现就如同换行（`NL`）符一样，用来终止一行输入并使该行对读取进程可见。默认情况下，这些字符是未定义的。如果定义了它们，它们是会被发送给读取进程的。`EOL2` 字符只有当设置了 `IEXTEN`（扩展输入处理）标志时（默认会设置）才能工作用到这些字符的机会很少。一种应用是在 telnet 中。通过将 `EOL` 或 `EOL2` 设定为 telnet 的换码符（通常是 `Ctrl+]`，或者如果工作在 `rlogin` 模式下时为`~`），telnet 能立刻捕获到字符，就算是正在规范模式下读取输入时也是如此

## ERASE

在规范模式下，输入 `ERASE` 字符会擦除当前行中前一个输入的字符。被擦除的字符以及 `ERASE` 字符本身都不会传递给读取输入的进程

## INTR

`INTR` 是中断字符。如果设置了 `ISIG` （开启信号）标志（默认会设置），输入这个字符会产生一个中断信号（`SIGINT`），并发送给终端的前台进程组。`INTR`字符本身是不会发送给读取输入的进程的

## KILL

`KILL` 是擦除行（也称为 `kill line`）字符。在规范模式下，输入这个字符使得当前这行输入被丢弃（即，到目前为止输入的字符连同 `KILL` 字符本身，都不会传递给读取输入的进程了）

## LNEXT

`LNEXT` 是下一个字符的字面化表示（literal next）。在某些情况下，我们可能希望将终端特殊字符的其他一个看作一个普通字符，将其作为输入传递给读取进程。输入 `LNEXT` 字符后（通常是 `Ctrl+V`）使得下一个字符将以字面形式来处理，避免终端驱动程序执行任何针对特殊字符的解释处理。因而，我们可以输入 `Ctrl+V ` 和 `Ctrl+C` 这样的 2 字符序列，提供一个真正的 `Ctrl+C` 字符（ASCII 码为 3）作为输入传递给读取进程。`LNEXT` 字符本身并不会传递给读取进程。这个字符只有在设定了 `IEXTEN` 标志（默认会设置）的规范模式下才会被解释

## NL

`NL` 是换行符。在规范模式下，该字符终结一行输入。`NL` 字符本身是会包含在行中返回给读取进程的（规范模式下，`CR` 字符通常会转换为 `NL`。）输出一个 `NL` 字符导致终端将光标移动到下一行。如果设置了 `OPOST` 和 `ONLCR`（将 `NL` 映射为 `CR-NL`）标志（默认会设置），那么在输出中，一个换行符就会映射为一个 2 字符序列— `CR` 加上 `NL`。（同时设定 `ICRNL` 和 `ONLCR` 标志意味着一个输入的 `CR` 字符会转换为 `NL`，然后回显为 `CR` 加上 `NL`

## QUIT

如果设置了 `ISIG` 标志（默认会设置），输入 `QUIT` 字符会产生一个退出信（`SIGQUIT`），并发送到终端的前台进程组中。`QUIT` 字符本身并不会传递给读取进程

## REPRINT

`REPRINT` 字符代表重新打印输入。在规范模式下，如果设置了 `IEXTEN` 标志（默认会设置），输入该字符会使得当前的输入行（还没有输入完全）重新显示在终端上。如果某个其他的程序（例如 `wall(1)` 或者 `write(1)`）输出已经使终端的显示变得混乱不堪，那么此时这个功能就特别有用了。`REPRINT` 字符本身是不会传递给读取进程的

## START 和 STOP

`START` 和 `STOP` 分别代表开始输出和停止输出字符。当设定了 `IXON` （启动开始/停止输出控制）标志时（默认会设定），这两个字符才能工作。（`START` 和 `STOP` 字符在一些终端模拟器中不会生效。

输入 `STOP` 字符会暂停终端输出。`STOP` 字符本身不会传递给读取进程。如果设定了 `IXOFF` 标志，而且终端的输入队列已满，那么终端驱动程序会自动发送一个`STOP` 字符来对输入进行字节流控制。

输入 `START` 字符会使得之前由 `STOP` 暂停的终端输出得到恢复。`START` 字符本身不会传递给读取进程。如果设定了 `IXOFF`（启动开始/停止输入控制）标志（默认是不会设定的），且终端驱动程序之前由于输入队列已满已经发送过了一个 `STOP` 字符，那么一旦当输入队列中又有了空间，此时终端驱动程序会自动发送一个 `START` 字符以恢复输出。

如果设定了 `IXANY` 标志，那么任何字符，不仅仅只是 `START`，都可以按顺序输入以重启输出（同样，这个字符也不会传递给读取进程）。

随着目前越来越普遍的高线速，软件流控已经被硬件流控（`RTS/CTS`）所取代了。在硬件流控中，通过串口上两条不同线缆上发送的信号来开启或关闭数据流。（`RTS` 代表请求发送，`CTS` 代表清除发送。

## SUSP

`SUSP` 代表暂停字符。如果设定了 `ISIG` 标志（默认会设定），输入这个字符会产生终端暂停信号（`SIGTSTP`），并发送给终端的前台进程组。`SUSP` 字符本身不会发送给读取进程。

## WERASE

`WERASE` 是擦除单词字符。在规范模式下，设定了 `IEXTEN` 标志（默认会设定）后输入这个字符会擦除前一个单词的所有字符。一个单词被看做是一串字符序列，可包含数字和下划线。（在某些 UNIX 实现中，单词被看做是由空格分隔的字符序列。

# 终端标志

`termios` 结构体中 4 个标志字段所控制的设置：

![](./img/terminal_flag1.png)
![](./img/terminal_flag2.png)
![](./img/terminal_flag3.png)
![](./img/terminal_flag4.png)

## BRKINT

- 如果设定了 `BRKINT`，且没有设定 `IGNBRK` 标志，那么当出现 `BREAK` 状态时会发送 `SIGINT` 信号到前台进程组
- 在许多 UNIX系统中，`BREAK` 状态就表现为一个发送给远端主机的信号，用来将线速（波特率）调整为适合于终端的数值
- 在虚拟控制台上，我们可以通过按下 `Ctrl+Break` 来产生一个 `BREAK` 状态

## ECHO

- 设置了 `ECHO` 标志将开启回显输入字符的功能。`ECHO` 标记在规范和非规范模式下都是有效的
- 当读取密码时，禁止回显是很有用的
- `vi` 的命令模式下回显也是被禁止的，此时由键盘产生的字符被解释为编辑命令而不是文本输入

## ECHOCTL

- 如果设置了 `ECHO` 标志，那么开启 `ECHOCTL` 标志会导致除了制表符、换行符、`START` 和 `STOP` 之外的控制字符都将以类似 `^A`（`Ctrl+A`）的形式回显出来
- 如果关闭 `ECHOCTL` 标志，控制字符将不再回显

## ECHOE

- 在规范模式下，设定ECHOE标识使得ERASE能以可视化的可视化执行，将退格-空格-退格格这样的序列输出到终端上。
- 如果关闭了 ECHOE 标志，那么 ERASE 字符本身就会回显出来（例如以^?的形式），但仍然会完成删除一个字符的功能

## ECHOK 和 ECHOKE

- `ECHOK` 和 `ECHOKE` 标志控制着在规范模式下使用 `KILL`（擦除行）字符时的可视化显示
- 在默认情况下（同时设置两个标志），一行文本以可视化的方式擦除（参见 `ECHOE`）
- 如果其中任一标志被关闭，那么就不会执行可视化的擦除（但输入行仍然会被丢弃），而 `KILL` 字符本身会被回显出来（例如以 `^U` 的形式）
- 如果设定了 `ECHOK` 而关闭了 `ECHOKE`，那么也会输出一个换行符

## ICANON

- 设定了 `ICANON` 标志将启动规范模式输入。
- 输入会集中成行，并且会打开对特殊字符 `EOF`、`EOL`、`EOL2`、`ERASE`、`LNEXT`、`KILL`、`REPRINT` 以及 `WERASE` 的解释处理（但需要注意下面描述到的 `IEXTEN` 标志所产生的效果）

## IEXTEN

- 设定 `IEXTEN` 标志将打开对输入字符的扩展处理功能
- 必须设定这个标志（同 `ICANON` 一样），才能正确解释 `EOL2`、`LNEXT`、`REPRINT` 以及 `WERASE` 这样的特殊字符
- 要使 `IUCLC` 标志生效，也必须要设定 `IEXTEN` 标志才行

## IMAXBEL

- Linux 上忽略了 `IMAXBEL` 标志的设定
- 在登录控制台上，当输入队列已满时总是会响起响铃声

## IUTF8

设定 `IUTF8` 标志将打开加工模式（cooked mode），以此当执行行编辑时能够正确地处理 UTF-8 输入

## NOFLSH

- 默认情况下，当输入 `INTR`、`QUIT` 或 `SUSP` 字符而产生信号时，任何在终端输入和输出队列中未处理完的数据都会被刷新（丢弃）
- 设定 `NOFLSH` 标志后将关闭这种刷新行为

## OPOST

- 设定 `OPOST` 标志后将打开输出的后续处理功能。
- 必须设定该标志才能使 `termios` 结构体中 `c_oflag` 字段中的标志生效。（相反，关闭 `OPOST` 标志将禁止对所有的输出做后续处理。）

## PARENB、IGNPAR、INPCK、PARMRK 以及 PARODD

`PARENB`、`IGNPAR`、`INPCK`、`PARMRK` 以及 `PARODD` 标志同奇偶校验生成和检查有关：

- `PARENB` 标志可为输出字符打开奇偶校验位，并为输入字符做奇偶校验检查
- 如果我们只希望生成输出的奇偶校验，那么我们可以通过关闭 `INPCK` 标志来禁止对输入做奇偶校验检查
- 如果设定了 `PARODD` 标志，那么在输入和输出上都会采用奇数奇偶校验，否则就会采用偶数奇偶校验

剩下的标志规定了当输入字符出现奇偶校验错误时应该如何处理。

- 如果设定了 `IGNPAR` 标志，那么字符将被丢弃（不会传递给读取进程）
- 否则，如果设定了 `PARMRK` 标志，那么该字符会传递给读取进程，但会在前面加上 2 字节的序列 0377 + 0。（如果设定了 `PARMRK` 标志，但关闭了 `ISTRIP` 标志，那么字符 0377 会加倍成 0377 + 0377。）
- 如果关闭 `PARMRK` 标志，但设定了 `INPCK` 标志，那么字符被丢弃，且不会传递给读取进程任何字节
- 如果 `IGNPAR`、`PARMRK` 或 `INPCK` 都没有设定，那么该字符会传递给读取进程



































