#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int sockfd = create_client_socket("127.0.0.1", 8888);
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
