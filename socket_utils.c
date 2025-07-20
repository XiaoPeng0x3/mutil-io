#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// 创建服务端 socket
int create_server_socket(int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // 设置地址重用
    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    return sockfd;
}

// 绑定 socket 到指定地址和端口
int bind_socket(int sockfd, const char *addr, int port) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (addr == NULL) {
        server_addr.sin_addr.s_addr = INADDR_ANY; // 所有网卡
    } else {
        if (inet_pton(AF_INET, addr, &server_addr.sin_addr) <= 0) {
            perror("inet_pton");
            return -1;
        }
    }

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        return -1;
    }

    return 0;
}

// 启动监听
int start_listening(int sockfd, int backlog) {
    if (listen(sockfd, backlog) < 0) {
        perror("listen");
        return -1;
    }
    return 0;
}

// 接受客户端连接
int accept_client(int server_sockfd, struct sockaddr_in *client_addr) {
    socklen_t addrlen = sizeof(struct sockaddr_in);
    int client_sock = accept(server_sockfd, (struct sockaddr *)client_addr, &addrlen);
    if (client_sock < 0) {
        perror("accept");
        return -1;
    }
    return client_sock;
}

// 读取客户端数据
int read_data(int client_sockfd, char *buffer, int bufsize) {
    int bytes = read(client_sockfd, buffer, bufsize - 1);
    if (bytes < 0) {
        perror("read");
        return -1;
    }
    buffer[bytes] = '\0'; // null-terminate
    return bytes;
}

// 发送数据
int send_data(int client_sockfd, const char *data, int datalen) {
    int bytes = send(client_sockfd, data, datalen, 0);
    if (bytes < 0) {
        perror("send");
        return -1;
    }
    return bytes;
}

// 创建并连接客户端 socket
int create_client_socket(const char *ip, int port) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    
    if (inet_pton(AF_INET, ip, &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        return -1;
    }

    return sockfd;
}
