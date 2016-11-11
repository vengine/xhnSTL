
#include "LinuxString.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* string_convert_to_utf8(const char* input)
{
    int len = strlen(input);
    char* ret = (char*)malloc(len + 1);
    memcpy(ret, input, len + 1);
    return ret;
}
