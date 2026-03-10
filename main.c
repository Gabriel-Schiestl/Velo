#include <stdio.h>
#include <string.h>
#include "server/server.h"
#include <sys/epoll.h>
#include <unistd.h>
#include "parser/parser.h"

#define EPOLL_EVENTS_SIZE 1024
#define CLIENT_BUFFER_SIZE 1024

int epfd;
int server_fd;

struct epoll_event create_epoll_event(int fd) {
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;

    return ev;
}

void handle_client_accept() {
    int client_fd = accept(server_fd, NULL, NULL);

    struct epoll_event client_event;
    client_event.events = EPOLLIN;
    client_event.data.fd = client_fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &client_event);
}

int main() {
    server_fd = init_server(INADDR_ANY, 9090, 10);

    epfd = epoll_create1(0);

    struct epoll_event server_ev = create_epoll_event(server_fd);

    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &server_ev);

    struct epoll_event events[EPOLL_EVENTS_SIZE];

    while(1) {
        int n = epoll_wait(epfd, events, EPOLL_EVENTS_SIZE, -1);

        for(int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            if(fd == server_fd) {
                handle_client_accept();
                continue;
            }
            
            char buffer[CLIENT_BUFFER_SIZE];

            int n_read = read(fd, buffer, sizeof(buffer));
            if(n_read == -1) {
                close(fd);
                continue;
            }

            handle_client_message(buffer, fd);
        }
    }
}