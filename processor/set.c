#include "../store/store.h"

char* process_insert(char* key, char* value, char *ttl) {
    Entry *entry = new_entry(key, value, ttl);
    printf("New entry created: %s", key);
    if(table_insert(key, entry) == -1) {
        return NULL;
    }
    print_table();
    return "SUCCESS";
}