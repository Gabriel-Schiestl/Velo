#include <stdio.h>
#include <string.h>
#include <server.h>

int main() {
    int server_fd = init_server(INADDR_ANY, 9090, 10);
}