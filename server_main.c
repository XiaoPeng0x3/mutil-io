#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <wait.h>

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

    while (1) {
        struct sockaddr_in client_addr;
        int client_fd = accept_client(server_fd, &client_addr);
        if (client_fd < 0) continue;

        pid_t pid = fork();
        if (pid < 0) {
            perror("创建进程失败！");
            exit(1);
        } else if (pid == 0) {
            // 子进程处理请求
            char buffer[1024];
            read_data(client_fd, buffer, sizeof(buffer));
            printf("收到: %s\n", buffer);
            send_data(client_fd, "你好，客户端！\n", 18);
            close(client_fd);
            exit(0);  // 子进程必须退出
        } else {
            // 父进程继续监听下一个连接，并清理子进程
            close(client_fd); // 父进程不处理这个 client_fd
            while (waitpid(-1, NULL, WNOHANG) > 0);  // 非阻塞清理僵尸子进程
        }
    }
    close(server_fd);
    return 0;
}
