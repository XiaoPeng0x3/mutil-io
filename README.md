# 说明
使用了`fork`来让子进程处理一系列的耗时操作，成功处理多个客户端连接的情况

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
