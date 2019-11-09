#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <cstdint>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "DataTypes.hpp"

//typedef uint32_t hash_table_CRC32;

/*
struct crc32 {
    static void generate_table(uint32_t(&table)[256]) {
        uint32_t polynomial = 0xEDB88320;
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t c = i;
            for (size_t j = 0; j < 8; j++) {
                if (c & 1) {
                    c = polynomial ^ (c >> 1);
                } else {
                    c >>= 1;
                }
            }
            table[i] = c;
        }
    }

    static uint32_t update(uint32_t(&table)[256], uint32_t initial, const void* buf, size_t len) {
        uint32_t c = initial ^ 0xFFFFFFFF;
        const uint8_t* u = static_cast<const uint8_t*>(buf);
        for (size_t i = 0; i < len; ++i) {
            c = table[(c ^ u[i]) & 0xFF] ^ (c >> 8);
        }
        return c ^ 0xFFFFFFFF;
    }
};
*/
u32 hash_djb2(unsigned char* str);
stringID operator"" _sid(const char *input, size_t s);

#endif
