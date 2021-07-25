# 取消一个线程

```
#include <pthread.h>

int pthread_cancel(pthread_t thread);
```

- `pthread_cancel()` 发出取消请求后会立即返回，不会等待目标线程退出

# 取消状态及类型

```
#include <pthread.h>

int pthread_setcancelstate(int state,int *oldstate);
int pthread_setcanceltype(int type,int *oldstype);
```

- `pthread_setcancelstate()` 可以设置线程的取消状态，`state` 可以取值：

  - `PTHREAD_CANCEL_DISABLE` ：线程不可取消，如果此类线程收到取消请求，则会将请求挂起，直至将线程的取消状态置为启用
  - `PTHREAD_CANCEL_ENABLE`：线程可以取消，这是新建线程取消状态的默认值

- 如果取消状态为 `PTHREAD_CANCEL_ENABLE`，那么可以使用 `pthread_setcanceltype()` 设置取消类型：

  - `PTHREAD_CANCEL_ASYNCHRONOUS`：可能会在任何时点取消线程，异步取消的应用场景很少
  - `PTHREAD_CANCEL_DEFERED`：取消请求保持挂起状态，直至达到取消点，这也是新建线程的缺省类型

# 取消点

若将线程的取消状态和类型分别置为启用和延迟，仅当线程抵达某个取消点时，取消请求才会起作用，取消点即是对由实现定义的一组函数之一加以调用。

SUSv3 规定必须是取消点的函数：

![](./img/cancel_point.png)

线程一旦收到取消请求，其启用了取消状态并将类型设置为 延迟，则其会在下次抵达取消点时终止，如果线程尚未 `detach`，那么为了防止其变为僵尸线程，必须由其他线程对其 `join`，`pthread_join()` 的第二个参数将收到 `PTHREAD_CANCELED`。

# 线程可取消的检测

```
#include <pthread.h>

void pthread_testcancel(void);
```

- `pthread_testcancel()` 的作用就是产生一个取消点，线程如果已有处于挂起状态的取消状态，那么调用该函数，线程就会随之终止
- 当线程执行的代码未包含取消点时，可以周期性地调用 `pthread_testcancel()`，以确保对其他线程向其发送的取消请求做出及时响应

# 清理函数

一旦有处于挂起状态的取消请求，线程在执行到取消点时，如果只是草草收场，这会将共享变量以及 Pthreads 对象(例如互斥量)置于一种不一致状态，可能导致进程中其他线程产生错误的结果，例如：死锁，甚至是崩溃。为规避这个问题，线程可以设置一个或多个清理函数，当线程遭取消时会自动运行这些函数，在线程终止前可以执行诸如修改全局变量，解锁互斥量等操作。

```
#include <pthread.h>

void pthread_cleanup_push(void(*routine)(void*),void* arg);
void pthread_cleanup_pop(int execute);
```

# 异步取消

如果设定线程为异步取消时，即取消类型设置为 `PTHREAD_CANCEL_ASYNCHRONOUS`，可以在任何时间点将其取消，取消动作不会拖延到下一个取消点才执行。

异步取消的问题在于，尽管清理函数依然会得以执行，但处理函数却无从得知线程的具体状态。作为一般性原则，可异步取消的线程不应该分配任何资源，也不能获取互斥量或锁。















