#include "socket_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX_CLIENTS 5
#define MAX_EVENTS 10
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("用法: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    // 创建、绑定、监听服务端 socket
    int server_fd = create_server_socket(port);
    if (server_fd < 0) return 1;
    if (bind_socket(server_fd, ip, port) < 0) return 1;
    if (start_listening(server_fd, MAX_CLIENTS) < 0) return 1;

    printf("监听 %s:%d 中，等待客户端...\n", ip, port);

    // 创建 epoll 实例
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        close(server_fd);
        return 1;
    }

    // 添加服务端 socket 到 epoll
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl: server_fd");
        close(server_fd);
        close(epfd);
        return 1;
    }

    char buffer[BUFFER_SIZE];

    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                // 新客户端连接
                struct sockaddr_in client_addr;
                int client_fd = accept_client(server_fd, &client_addr);
                if (client_fd < 0) continue;

                printf("新客户端连接：%s:%d (fd=%d)\n",
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), client_fd);

                // 将客户端添加到 epoll
                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                    perror("epoll_ctl: client_fd");
                    close(client_fd);
                }
            } else {
                // 有客户端数据可读
                int bytes = read_data(fd, buffer, BUFFER_SIZE);
                if (bytes <= 0) {
                    printf("客户端断开连接: fd=%d\n", fd);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                } else {
                    printf("收到(fd=%d): %s\n", fd, buffer);
                    // 回显数据
                    send_data(fd, buffer, strlen(buffer));
                }
            }
        }
    }

    close(server_fd);
    close(epfd);
    return 0;
}
