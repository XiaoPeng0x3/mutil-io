# 前言
我们来跟随历史的脚步，看看前辈是如何对网络阻塞IO进行优化的。

# 遇到的问题
众所周知，如果不对网络`IO`进行优化，那么服务器只可以处理一个客户端的连接，如果这个客户端在进行一些耗时操作，那么其它的客户端是不可以进行连接的。

# 说明
在`main`分支中，只是实现了最简单的客户端连接，未对`IO`进行任何优化

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
