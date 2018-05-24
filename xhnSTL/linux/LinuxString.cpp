
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

const char* convert_utf8_path_to_url(const char* utf8_path)
{
    return nullptr;
}

const char* convert_url_to_utf8_path(const char* url)
{
    return nullptr;
}