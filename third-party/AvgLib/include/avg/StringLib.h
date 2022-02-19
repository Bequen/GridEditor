#pragma once

#include <cstring>
#include <cstdint>

namespace avg {
    namespace StringLib {
        /**
         * @brief  Splits string into multiple strings
         * @note   Copies the string, so the original will not be hurt
         * @param  str: string to be splitted
         * @param  splitter: character that will be used as splitter
         * @param  count: number will receive number of splits
         * @retval returns actual array of splits
         */
        char** split(const char* str, const char splitter, uint32_t* count);
        /**
         * @brief  Splits string into multiple strings
         * @note   Copies the string, so the original will not be hurt
         * @param  str: string to be splitted
         * @param  splitters: characters that will be used as splitters
         * @param  splitterCount: number of splitters
         * @param  splitCount: number will receive number of splits
         * @retval returns actual array of splits
         */
        char** split(char* str, const char* splitters, uint32_t splitterCount, uint32_t* splitCount);

        /**
         * @brief  Concatenate two strings
         * @note   
         * @param  str1: String number one
         * @param  str2: String number two
         * @retval returns concatenated string
         */
        char* concat(const char* str1, const char* str2);

        /**
         * @brief  Tests wheter two strings are equal
         * @note   
         * @param  str1: String number one
         * @param  str2: String number two
         * @retval returns wheter two strings are equal
         */
        bool equals(const char* str1, const char* str2);
        /**
         * @brief  Tests wheter string starts with some text
         * @note   
         * @param  str: String to be tested
         * @param  startWith: Testing value
         * @retval returns wheter string starts with the other
         */
        bool startWith(const char* str, const char* startWith);

        /**
         * @brief  Tests wheter string ends with some text
         * @note   
         * @param  str: String to be tested
         * @param  startWith: Testing value
         * @retval returns wheter string ends with the other
         */
        bool endWith(const char* str, const char* endWith);

        /**
         * @brief  Gets length of the char array
         * @note   
         * @param  str: string which's length we want to get
         * @retval returns length of the char array
         */
        uint32_t length(char* str);

        /**
         * @brief  Gets length of the char array
         * @note   
         * @param  str: string which's length we want to get
         * @retval returns length of the char array
         */
        uint32_t length(const char* str);
    };
};