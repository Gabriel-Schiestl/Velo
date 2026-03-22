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
    } else if(strcmp(cmd, "REMAINING") == 0) {
        char *resp = process_remaining(key);
        send_success(resp, fd, 1);
        free(resp);
    } else {
        send_error("command not allowed", fd, 1);
    }
}

void handle_client_message(char* message, int client_fd) {
    printf("Message received: %s", message);
    int index = 0;

    char cmd_len_char = message[index];
    int cmd_len;

    sscanf(&cmd_len_char, "%d", &cmd_len);

    char *cmd = strndup(message+1, cmd_len);
    index += cmd_len+1;

    char key_len_char = message[index];
    int key_len;

    sscanf(&key_len_char, "%d", &key_len);

    char *key = strndup(message+index+1, key_len);
    index += key_len+1;

    char val_exists_char = message[index];
    int val_exists;

    sscanf(&val_exists_char, "%d", &val_exists);

    index++;
    char *val;

    if(val_exists == 1) {
        char val_len_char = message[index];
        uint16_t val_len_big_end;

        int val_len_int = sscanf(&val_len_char, "%" SCNu16, &val_len_big_end);
        uint16_t val_len = ntohs(val_len_big_end);

        val = strndup(message+index+1, val_len);
        index += val_len+1;
    }
    printf("Value: %s", val);
    printf("%d\n", index);
    char ttl_exists_char = message[index];
    int ttl_exists;

    sscanf(&ttl_exists_char, "%d", &ttl_exists);
    printf("TTL EXISTS %d, %c\n", ttl_exists, ttl_exists_char);
    index++;
    uint32_t ttl = 0;

    if(ttl_exists == 1) {
        uint32_t ttl_big_end;

        sscanf(message+index, "%" SCNu32, &ttl_big_end);
        ttl = ntohs(ttl_big_end);
    }
    printf("AAAAAA %u\n", ttl);
    if(!cmd) {
        send_error("command not received", client_fd, 1);
        return;
    }

    if(!key) {
        send_error("key not received", client_fd, 1);
        return;
    }

    printf("%s, %s, %s, %u\n",
        cmd,
        key,
        val ? val : "NULL",
        ttl
    );

    process_command(cmd, key, val, ttl, client_fd);
}



