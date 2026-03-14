
#ifndef STORE_H
#define STORE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

struct Entry {
    char* key;
    char* value;
    time_t expire_at;
    int has_ttl;
    struct Entry* next;
};

typedef struct Entry Entry;

int8_t table_insert(const char* key, Entry *value);
int8_t table_delete(const char* key);
Entry* table_get(const char* key);
Entry* new_entry(const char* key, char* value, int *ttl);
Entry* table_expire(const char* key);

void init_table();
void print_table();
#endif