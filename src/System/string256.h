#pragma once

#include <cstdint>

/*
 * Size limited string
 */
struct string256 {
    char str[256];
    uint32_t length = 0;
};

string256 string256_concat(string256 str1, char* str2);