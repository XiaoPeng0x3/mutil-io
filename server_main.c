#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

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

        char buffer[1024];
        read_data(client_fd, buffer, sizeof(buffer));
        printf("收到: %s\n", buffer);

        send_data(client_fd, "你好，客户端！\n", 18);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
