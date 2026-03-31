#include "../store/store.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

char* process_remaining(char* key) {
    Entry *result = table_get(key);
    if(!result) {
        return NULL;
    }

    long remaining = -1;
    if(result->has_ttl) {
        remaining = (long)(result->expire_at - time(NULL));
        if(remaining < 0) {
            remaining = -1;
        }
    }

    int n = snprintf(NULL, 0, "OK %ld", remaining);

    char *response = malloc(n + 1);
    if (!response) return NULL;

    snprintf(response, n + 1, "OK %ld", remaining);

    return response;
}
