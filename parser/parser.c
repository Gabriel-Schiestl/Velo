#include <unistd.h>
#include <string.h>
#include "../processor/processor.h"
#include <stdio.h>
#include <stdlib.h>

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

void process_command(char *cmd, char *key, char *value, char *ttl, int fd) {
    char *end;
    long ttl_number = strtol(ttl, &end, 10);

    if (end == value) {
        send_error("invalid ttl number", fd, 1);
        return;
    }

    if(strcmp(cmd, "SET") == 0) {
        printf("Calling insertion\n");
        char *resp = process_insert(key, value, &ttl_number);
        if(resp) {
            send_success(resp, fd, 1);
            return;
        }

        send_error("could not insert new entry", fd, 1);
    } 
    else if(strcmp(cmd, "GET") == 0) {
        printf("Calling getter\n");
        char *resp = process_select(key);
        if(resp) {
            send_success(resp, fd, 1);
            free(resp);
            return;
        }

        send_error("could not get entry with this key", fd, 1);
    } else if(strcmp(cmd, "DELETE") == 0) {
        printf("Calling deletion\n");
        char *resp = process_delete(key);
        if(!resp) {
            send_error("could not delete entry", fd, 1);
            return;
        }

        send_success(resp, fd, 1);
    } else if(strcmp(cmd, "EXPIRE") == 0) {
        printf("Calling expiration\n");
        char *resp = process_expire(key);
        if(resp) {
            send_success(resp, fd, 1);
            free(resp);
            return;
        }

        send_error("could not expire entry", fd, 1);
    } 
    // else if(strcmp(cmd, "REST")) {
    //     char *resp = process_insert(key, value);
    //     send_success(resp, fd, 1);
    // } else {

    // }
}

void handle_client_message(char* message, int client_fd) {
    printf("Message received: %s", message);

    char *saveptr;

    char *cmd = strtok_r(message, " \r\n", &saveptr);
    char *key = strtok_r(NULL, " \r\n", &saveptr);
    char *value = strtok_r(NULL, " \r\n", &saveptr);
    char *ttl = strtok_r(NULL, " \r\n", &saveptr);

    if(!cmd) {
        send_error("command not received", client_fd, 1);
        return;
    }

    if(!key) {
        send_error("key not received", client_fd, 1);
        return;
    }

    printf("%s, %s, %s, %s\n",
        cmd,
        key,
        value ? value : "NULL",
        ttl ? ttl : "NULL"
    );

    process_command(cmd, key, value, ttl, client_fd);
}



