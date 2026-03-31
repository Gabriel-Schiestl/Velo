#include "../store/store.h"
#include <stdint.h>
#include <stdio.h>

char* process_insert(char* key, char* value, uint64_t ttl) {
    Entry *entry = new_entry(key, value, ttl);
    if (!entry) {
        return NULL;
    }
    printf("New entry created: %s\n", key);
    if(table_insert(key, entry) == -1) {
        return NULL;
    }
    print_table();
    return "OK";
}