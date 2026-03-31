#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define INITIAL_TABLE_SIZE 50
#define MAX_KEY_SIZE 50
#define TABLE_RESIZE_NEEDED 90

typedef struct Entry {
    char* key;
    char* value;
    time_t expire_at;
    int has_ttl;
    struct Entry* next;
} Entry;

Entry **table;
int table_size;
int element_count;

int hash(const char *key) {
    int hash_value = 0;
    for(int i = 0; i < strlen(key); i++) {
        hash_value += key[i];
        hash_value = (hash_value * key[i]) % table_size;
    }

    return hash_value;
}

int resize_needed() {
    return ((element_count * 100) / table_size) >= TABLE_RESIZE_NEEDED ? 0 : -1;
}

int resize_table() {
    int old_size = table_size;
    Entry **old_table = table;

    int new_size = old_size * 2;
    table = calloc(new_size, sizeof(Entry*));
    if (table == NULL) {
        table = old_table;
        return -1;
    }

    table_size = new_size;

    for (int i = 0; i < old_size; i++) {
        Entry *curr = old_table[i];
        while (curr) {
            Entry *next = curr->next;
            int new_index = hash(curr->key);
            curr->next = table[new_index];
            table[new_index] = curr;
            curr = next;
        }
    }

    free(old_table);
    return 0;
}

int8_t table_insert(const char* key, Entry *value) {
    if(strlen(key) == 0) {
        return -1;
    }

    element_count++;
    int index = hash(key);
    value->next = table[index];
    table[index] = value;

    if(resize_needed() != -1) {
        resize_table();
    }

    return 0;
}

int8_t table_delete(const char* key) {
    if (strlen(key) == 0) {
        return -1;
    }

    int index = hash(key);

    Entry **current = &table[index];

    while (*current) {

        Entry *node = *current;

        if (strcmp(node->key, key) == 0) {

            *current = node->next;

            free(node->key);
            free(node->value);
            free(node);
            element_count--;

            continue;
        }

        current = &node->next;
    }

    return 0;
}

Entry* table_get(const char* key) {
    if (strlen(key) == 0) {
        return NULL;
    }
    int index = hash(key);

    Entry* current = table[index];

    while(current) {
        printf("Checking key: '%s'\n", current->key);

        int cmp = strcmp(current->key, key);

        if(cmp == 0) {
            if(current->has_ttl && time(NULL) >= current->expire_at) {
                current = current->next;
                continue;
            }

            return current;
        }
        current = current->next;
    }

    return NULL;
}

Entry* table_expire(const char* key) {
    if (strlen(key) == 0) {
        return NULL;
    }
    int index = hash(key);

    Entry* current = table[index];

    while(current) {
        printf("Checking key: '%s'\n", current->key);

        int cmp = strcmp(current->key, key);

        if(cmp == 0) {
            current->expire_at = -1;
            current->has_ttl = 1;
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void print_table() {
    for(int i = 0; i < table_size; i++) {
        Entry *curr = table[i];
        if(curr == NULL) {
            printf("%d ----\n", i);
            continue;
        }

        printf("%d - ", i);
        while(curr) {
            if(curr->expire_at == -1) {
                printf("%s (expired) \t\t", curr->key);
            } else {
                printf("%s\t\t", curr->key);
            }
            curr = curr->next;
        }
        printf("\n");
    }
    printf("--------------\n");
}

Entry* new_entry(const char* key, char* value, uint64_t ttl) {
    Entry *e = malloc(sizeof(Entry));
    if (!e) return NULL;

    char *e_key = strndup(key, MAX_KEY_SIZE);

    e->key = e_key;
    e->value = strdup(value);
    e->next = NULL;

    if(ttl) {
        e->expire_at = time(NULL) + ttl;
        e->has_ttl = 1;
    } else {
        e->has_ttl = 0;
        e->expire_at = 0;
    }

    return e;
}

void init_table() {
    table = calloc(INITIAL_TABLE_SIZE, sizeof(Entry*));
    table_size = INITIAL_TABLE_SIZE;
}