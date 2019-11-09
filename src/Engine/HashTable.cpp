#include "HashTable.hpp"

u32 hash_djb2(unsigned char* str) {
    u32 hash = 5381;
    int c;

    while (c = *str++) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

stringID operator"" _sid(const char *input, size_t s) {
    /*unsigned char* k = (unsigned char*)malloc(s);
    strcpy((char*)k, input);
    stringID id = hash_djb2(k);
    //free(k);
    return id;*/

    return hash_djb2((unsigned char*)input);
}