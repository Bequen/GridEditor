#include "string256.h"

#include <avg/Debug.h>

string256 string256_concat(string256 str1, char* str2) {
    for(uint32_t i = 0; str2[i] != '\0'; i++) {
        str1.str[str1.length] = str2[i];
        str1.length++;
    }

    str1.str[str1.length] = '\0';

    return str1;
}