
#ifndef SERVER_H
#define SERVER_H
#include <sys/socket.h>
#include <netinet/in.h>
int init_server(in_addr_t addr, int port, int max_queue_size);
#endif