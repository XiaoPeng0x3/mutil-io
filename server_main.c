#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("用法: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int server_fd = create_server_socket(port);
    if (server_fd < 0) return 1;
    if (bind_socket(server_fd, ip, port) < 0) return 1;
    if (start_listening(server_fd, 5) < 0) return 1;

    printf("监听 %s:%d 中，等待客户端...\n", ip, port);

    int client_fds[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) client_fds[i] = -1;

    fd_set read_fds;
    int max_fd;

    char buffer[BUFFER_SIZE];

    while (1) {
        // bitmap 清零
        FD_ZERO(&read_fds);
        // 把bitmap绑定到 server的套接字中
        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;
        
        // 把客户端套接字绑定到bitmap里面，并更新max_fd
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] != -1) {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd) max_fd = client_fds[i];
            }
        }
        // 开始select
        int ret = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select");
            break;
        }

        // 有新连接
        if (FD_ISSET(server_fd, &read_fds)) {
            struct sockaddr_in client_addr;
            int client_fd = accept_client(server_fd, &client_addr);
            if (client_fd >= 0) {
                int added = 0;
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (client_fds[i] == -1) {
                        client_fds[i] = client_fd;
                        added = 1;
                        break;
                    }
                }
                if (!added) {
                    printf("客户端已满，拒绝连接 fd=%d\n", client_fd);
                    close(client_fd);
                }
            }
        }

        // 处理客户端数据
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];
            if (fd != -1 && FD_ISSET(fd, &read_fds)) {
                memset(buffer, 0, sizeof(buffer));
                int n = read_data(fd, buffer, sizeof(buffer));
                if (n <= 0) {
                    printf("客户端 fd=%d 断开连接\n", fd);
                    close(fd);
                    client_fds[i] = -1;
                } else {
                    printf("收到 fd=%d: %s\n", fd, buffer);
                    send_data(fd, "你好，客户端！\n", 18);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
