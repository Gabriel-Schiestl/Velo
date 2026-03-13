#include "../store/store.h"

char* process_select(char* key) {
    Entry *result = table_get(key);

    if(!result)
        return NULL;

    size_t len = strlen(result->value) + strlen("SUCCESS ") + 1;

    char *resp = malloc(len);

    snprintf(resp, len, "SUCCESS %s", result->value);

    return resp;
}