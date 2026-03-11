#include <unistd.h>
#include <string.h>

void send_error(char* message, int fd, int _close) {
    write(fd, "err: ", 5);
    write(fd, message, strlen(message));
    write(fd, "\r\n", 2);

    if(_close) {
        close(fd);
    }
}

int get_blank_index(char *message) {
    int i = 0;
    while(message[i] != ' ' && message[i] != '\0') {
        i++;
    }

    return i;
}

void handle_client_message(char* message, int client_fd) {
    int cmd_idx = get_blank_index(message);
    if (cmd_idx == 0) {
        send_error("command not received", client_fd, 1);
        return;
    }

    message[cmd_idx] = '\0';

    char *cmd = message;
    char *rest = message + cmd_idx + 1;

    int key_idx = get_blank_index(rest);
    if (key_idx == 0) {
        send_error("key not received", client_fd, 1);
        return;
    }

    rest[key_idx] = '\0';

    char *key = rest;
    char *value = rest + key_idx + 1;
}



