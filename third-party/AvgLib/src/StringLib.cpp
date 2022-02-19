#include "avg/StringLib.h"

#include <iostream>


char** avg::StringLib::split(const char* str, const char splitter, uint32_t* splitCount) {
    uint32_t length = 0;
    for( ; str[length] != '\0'; length++) {
        if(str[length] == splitter && (str[length - 1] != splitter)) {
            *splitCount += 1;
        }
    }

    *splitCount += 1;

    char* resultStr = new char[length + 1];
    memcpy(resultStr, str, length);
    resultStr[length] = '\0';

    char** splits = new char*[*splitCount];
    uint32_t index = 1;

    splits[0] = &resultStr[0];
    for(uint32_t i = 0; i < length; i++) {
        if(str[i] == splitter) {
            resultStr[i] = '\0';
            splits[index] = &resultStr[i + 1];
            if(resultStr[i - 1] == '\0') {
                splits[index - 1]++;
            } else {
                index++;
            }
        }
    }

    return splits;
}

char** avg::StringLib::split(char* str, const char* splitters, uint32_t splitterCount, uint32_t* splitCount) {
    for(uint32_t i = 0; str[i] != '\0'; i++) {
        for(uint32_t x = 0; x < splitterCount; x++) {
            if(str[i] == splitters[x])
                str[i] = splitters[0];
        }
    }
    
    uint32_t length = 0;
    for( ; str[length] != '\0'; length++) {
        if(str[length] == splitters[0] && (str[length - 1] != splitters[0])) {
            *splitCount += 1;
        }
    }

    *splitCount += 1;

    char* resultStr = new char[length + 1];
    memcpy(resultStr, str, length);
    resultStr[length] = '\0';

    char** splits = new char*[*splitCount];
    uint32_t index = 1;

    splits[0] = &resultStr[0];
    for(uint32_t i = 0; i < length; i++) {
        if(str[i] == splitters[0]) {
            resultStr[i] = '\0';
            splits[index] = &resultStr[i + 1];
            if(resultStr[i - 1] == '\0') {
                splits[index - 1]++;
            } else {
                index++;
            }
        }
    }

    return splits;
}

char* avg::StringLib::concat(const char* str1, const char* str2) {
    uint32_t length1 = StringLib::length(str1);
    uint32_t length2 = StringLib::length(str2);

    char* result = new char[length1 + length2];
    memcpy(result, str1, length1);
    memcpy(result + length1, str2, length2);

    return result;
}

bool avg::StringLib::equals(const char* str1, const char* str2) {
    uint32_t i = 0;

    for(; str1[i] == str2[i]; i++)
        if(str1[i] == '\0')
            return true;

    return false;
}

uint32_t avg::StringLib::length(char* str) {
    uint32_t i = 0;
    for(; str[i] != '\0'; i++) { }

    return i;
}

uint32_t avg::StringLib::length(const char* str) {
    uint32_t i = 0;
    for(; str[i] != '\0'; i++) { }

    return i;
}

bool avg::StringLib::startWith(const char* str, const char* start) {
    for(uint32_t i = 0; ; i++) {
        if(start[i] == '\0')
            return true;
        else if(str[i] != start[i])
            return false;
    }
}

bool avg::StringLib::endWith(const char* str, const char* end) {
    uint32_t length = avg::StringLib::length(str);
    uint32_t endLength = avg::StringLib::length(end);
    for(uint32_t i = 0; ; i++) {
        if(end[i] == '\0')
            return true;
        else if(str[length - endLength + i] != end[i])
            return false;
    }
}