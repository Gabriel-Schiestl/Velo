#include "../store/store.h"

char* process_expire(char* key) {
    Entry *result = table_expire(key);
    if(!result) {
        return NULL;
    }

    int n = snprintf(NULL, 0, "%s %s", "SUCCESS", result->key);

    char *response = malloc(n + 1);

    snprintf(response, n + 1, "%s %s", "SUCCESS", result->key);

    return response;
}