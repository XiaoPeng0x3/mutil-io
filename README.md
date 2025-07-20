# 说明
我们来看看怎么实现一个`select`服务端。

# select的实现
## select 干了什么事？
`select` 是一种 IO 多路复用技术，通常用于单进程或单线程模型下，同时监听多个客户端连接。  
一句话概括：**IO 多路复用就是让一个线程同时等待多个 IO 事件就绪，从而避免为每个连接创建线程或进程。**

在传统的阻塞式 TCP 服务器中，服务端在读取某个客户端数据时会被阻塞，导致无法处理其他客户端请求。而使用 IO 多路复用技术后，服务端可以同时监听多个 socket 文件描述符，**只在有数据可读时才真正去处理对应客户端**，从而避免了不必要的阻塞。

这就像你在等多个电话打进来，而不是每次只能拿起一个电话等别人说完再接下一个 —— 多路复用的好处就是：**你只在真正有人找你时才去接听**，这样就不会浪费时间了。

## 用法
首先来看看`select`的签名
```bash
man 2 select
```

```C
int select(int nfds, fd_set *_Nullable restrict readfds,
                  fd_set *_Nullable restrict writefds,
                  fd_set *_Nullable restrict exceptfds,
                  struct timeval *_Nullable restrict timeout);
```

有很多参数，对吧。第一次看到可能会觉得比较复杂，来说一说每个参数都是什么意思

- nfds
    原文档里面是这样说的
    > This argument should be set to the highest-numbered file descriptor in any of the three sets, plus 1.
    意思就是说，`select`所处理的客户端连接是有数量限制的，这是因为一个进程默认条件下最多可以有1024个打开的文件描述符(范围是`[0..1023]`)，而且，为了方便记录打开的文件描述符的具体数值，这里使用的是`bitmap`进行记录；
    假设客户端连接的文件描述符是`1, 3, 2, 4, 5`，这`5`个数用`bitmap`来进行表示就需要`6`位，也就是`max(fd) = 5`, 就是`max_fd + 1`，所以这就是这个参数的计算方法。
- xxxfds
    这个参数是指什么条件下`select`可以停止阻塞，从名字上不难看出，如果希望有读取、写入或预期事件发生时，就传递对应参数即可，一般情况下我们可以只传递`readfds`这个参数

- timeout
    也就是超时时间，在超时时间后，必须返回。这个值设置为`NULL`时表示永远等待。


## select 的实现细节
`select`就是去监听客户端连接套接字，然后，把这些客户端套接字拷贝到内核中，由内核进行监听，当内核监听到有数据准备完毕，就会从`select`中返回，也就是把对应文件描述符的`bitmap`置为1。但是缺点在于，我们不知道究竟是哪个客户端就绪了，所以只能去循环遍历寻找就绪的文件描述符，这也是`select`效率较低的原因，而且，`select`还不能很好的复用这个`bitmap`，导致每次连接完之后，在循环中必须得重新将`bitmap`全部置0。
总结一下`select`的原理
- 维护一定数量的客户端文件描述符
- 将对应的描述符拷贝到内核中，由内核监听
- 当有一个文件描述符接收到数据时，就把对应的`bitmap`的那一位设置为1，然后把这个`bitmap`返回给用户应用
- 用户遍历寻找`bitmap`置为1的那个文件描述符，从而进行数据的读写。

## select的缺点
缺点也显而易见了
- 一般情况下，最多只能进行`1024`个连接
- 需要数据拷贝
- 需要循环遍历确实就绪的文件描述符，效率低


# 构建/Build

首先`clone`本项目
```bash
git clone https://github.com/XiaoPeng0x3/mutil-io.git
```

然后执行
```bash
make clean
```
## 构建server
首先执行
```bash
make server
```
这样可以在`/target`目录下生成我们的`server`。

`server`的运行需要接受两个参数：`ip`和`post`,例如，我们想要运行一个`server`，可以执行
```bash
# 在本机的8081端口运行服务程序
./target/server 127.0.0.1 8081
```
这样就构建成功了！

## 构建client
同样的，可以这样构建
```bash
make client
```
连接一个已经运行的服务，需要指明`ip`和端口号

```bash
./target/client 127.0.0.1 8081
```
这样就可以愉快的进行连接了！
