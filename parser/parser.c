#include <unistd.h>
#include <string.h>
#include "../processor/processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <inttypes.h>

void send_error(char* message, int fd, int _close) {
    write(fd, "err: ", 5);
    write(fd, message, strlen(message));
    write(fd, "\r\n", 2);

    if(_close) {
        close(fd);
    }
}

void send_OK(char* message, int fd, int _close) {
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

void process_command(char *cmd, char *key, char *value, uint64_t ttl, int fd) {
    if(strcmp(cmd, "SET") == 0) {
        printf("Calling insertion\n");
        char *resp = process_insert(key, value, ttl);
        if(resp) {
            send_OK(resp, fd, 1);
            return;
        }

        send_error("could not insert new entry", fd, 1);
    } 
    else if(strcmp(cmd, "GET") == 0) {
        printf("Calling getter\n");
        char *resp = process_select(key);
        if(resp) {
            send_OK(resp, fd, 1);
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

        send_OK(resp, fd, 1);
    } else if(strcmp(cmd, "EXPIRE") == 0) {
        printf("Calling expiration\n");
        char *resp = process_expire(key);
        if(resp) {
            send_OK(resp, fd, 1);
            free(resp);
            return;
        }

        send_error("could not expire entry", fd, 1);
    } 
    // else if(strcmp(cmd, "REMAINING") == 0) {
    //     char *resp = process_remaining(key);
    //     send_success(resp, fd, 1);
    //     free(resp);
    // } else {
    //     send_error("command not allowed", fd, 1);
    // }
}

void handle_client_message(char* message, int client_fd) {
    int index = 0;

    int cmd_len = (unsigned char) message[index++];
    char *cmd = strndup(message + index, cmd_len);
    index += cmd_len;

    int key_len = (unsigned char) message[index++];
    char *key = strndup(message + index, key_len);
    index += key_len;

    int val_exists = (unsigned char) message[index++];

    char *val = NULL;

    if (val_exists == 1) {
        uint16_t val_len;
        memcpy(&val_len, message + index, sizeof(uint16_t));
        val_len = ntohs(val_len);
        index += sizeof(uint16_t);

        val = strndup(message + index, val_len);
        index += val_len;
    }

    int ttl_exists = (unsigned char) message[index++];

    uint32_t ttl = 0;

    if (ttl_exists == 1) {
        uint32_t ttl_big_end;
        memcpy(&ttl_big_end, message + index, sizeof(uint32_t));
        ttl = ntohl(ttl_big_end);
        index += sizeof(uint32_t);
    }

    printf("%s, %s, %s, %u\n",
        cmd,
        key,
        val ? val : "NULL",
        ttl
    );

    process_command(cmd, key, val, ttl, client_fd);

    free(cmd);
    free(key);
    if (val) free(val);
}



