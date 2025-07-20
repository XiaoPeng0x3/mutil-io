#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("用法: %s <ip> <port>\n", argv[0]);
        return 1;
    }
    // 获取IP
    const char *ip = argv[1];
    // 获取port
    int port = atoi(argv[2]);
    int sockfd = create_client_socket(ip, port);
    if (sockfd < 0) return 1;
    char buffer[1024];
    printf("输入一些东西吧: ");
    scanf("%s", buffer);
    send_data(sockfd, buffer, 18);

    read_data(sockfd, buffer, sizeof(buffer));
    printf("收到服务端: %s\n", buffer);

    close(sockfd);
    return 0;
}
