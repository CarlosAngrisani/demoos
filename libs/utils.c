#include "utils.h"

size_t strlen(const char* str) {
    size_t len = 0;
    
    int i = 0;
    while (str[i] != '\0') {
        len++;
    }

    return len;
}