#include <unistd.h>
#include <string.h>
#include "../processor/processor.h"
#include <stdio.h>

void send_error(char* message, int fd, int _close) {
    write(fd, "err: ", 5);
    write(fd, message, strlen(message));
    write(fd, "\r\n", 2);

    if(_close) {
        close(fd);
    }
}

void send_success(char* message, int fd, int _close) {
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

void process_command(char *cmd, char *key, char *value, int fd) {
    if(strcmp(cmd, "SET") == 0) {
        printf("Calling insertion\n");
        char *resp = process_insert(key, value);
        if(resp) {
            send_success(resp, fd, 1);
            return;
        }

        send_error("could not insert new entry", fd, 1);
    } 
    // else if(strcmp(cmd, "GET")) {
    //     char *resp = process_insert(key, value);
    //     send_success(resp, fd, 1);
    // } else if(strcmp(cmd, "DELETE")) {
    //     char *resp = process_insert(key, value);
    //     send_success(resp, fd, 1);
    // } else if(strcmp(cmd, "EXPIRE")) {
    //     char *resp = process_insert(key, value);
    //     send_success(resp, fd, 1);
    // } else if(strcmp(cmd, "REST")) {
    //     char *resp = process_insert(key, value);
    //     send_success(resp, fd, 1);
    // } else {

    // }
}

void handle_client_message(char* message, int client_fd) {
    printf("Message received: %s", message);

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

    printf("%s, %s, %s", cmd, key, value);

    process_command(cmd, key, value, client_fd);
}



