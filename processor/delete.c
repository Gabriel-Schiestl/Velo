#include "../store/store.h"

char* process_delete(char* key) {
    int8_t result = table_delete(key);
    if(result == -1) {
        return NULL;
    }
    print_table();
    return "OK";
}