#include "../store/store.h"

char* process_insert(char* key, char* value) {
    Entry *entry = new_entry(key, (void*)value, NULL);
    printf("New entry created: %s", key);
    if(table_insert(key, entry) == -1) {
        return NULL;
    }

    return "SUCCESS";
}