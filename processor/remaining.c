// #include "../store/store.h"

// char* process_remaining(char* key) {
//     Entry *result = table_get(key);
//     if(!result) {
//         return NULL;
//     }

//     long remaining = 0;
//     if(result->has_ttl) {
//         if(result->expire_at <= time(NULL)) {
//             remaining = -1;
//         } else {
//             remaining = time(NULL) - result->expire_at;
//         }
//     } 

//     int n = snprintf(NULL, 0, "%s %ld", "SUCCESS", remaining);

//     char *response = malloc(n + 1);

//     snprintf(response, n + 1, "%s %ld", "SUCCESS", remaining);

//     return response;
// }