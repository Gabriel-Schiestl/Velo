#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

#define err(code) exit(code)

int init_server(in_addr_t addr, int port, int max_queue_size) {
    int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if(sock_fd == -1) {
        printf("Error creating socket");
        err(-1);
    }

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = addr;

    int bnd = bind(sock_fd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if(bnd == -1) {
        printf("Error binding socket");
        err(-1);
    }

    int listn = listen(sock_fd, max_queue_size);
    if(listn == -1) {
        printf("Error listening socket");
        err(-1);
    }

    printf("Server listening on port %d\n", port);

    return sock_fd;
}