#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <netinet/in.h>  // struct sockaddr_in
#include <sys/socket.h>  // socket functions

// 创建服务端 socket，返回 socket fd，失败返回 -1
int create_server_socket(int port);

// 绑定 socket 到指定地址和端口（addr 为 IP 字符串，port 为端口号）
// 返回 0 表示成功，-1 表示失败
int bind_socket(int sockfd, const char *addr, int port);

// 开始监听 socket，backlog 为等待队列长度
// 返回 0 表示成功，-1 表示失败
int start_listening(int sockfd, int backlog);

// 等待客户端连接，成功返回 client socket fd，失败返回 -1
int accept_client(int server_sockfd, struct sockaddr_in *client_addr);

// 读取客户端发送的数据，返回实际读取的字节数
// buffer 为接收缓冲区，bufsize 为缓冲区大小
int read_data(int client_sockfd, char *buffer, int bufsize);

// 发送数据到客户端，返回实际发送字节数
int send_data(int client_sockfd, const char *data, int datalen);

// 创建客户端 socket，并连接指定服务器
// 成功返回 socket fd，失败返回 -1
int create_client_socket(const char *ip, int port);

#endif // SOCKET_UTILS_H
