#include <stdint.h>

#ifndef PROCESSOR_H
#define PROCESSOR_H
char* process_insert(char* key, char* value, uint64_t ttl);

char* process_select(char* key);

char* process_delete(char* key);

char* process_expire(char* key);
#endif