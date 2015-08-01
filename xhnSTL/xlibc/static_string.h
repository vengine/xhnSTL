#ifndef STATIC_STRING_H
#define STATIC_STRING_H
#include "common.h"
#include "etypes.h"
typedef struct
{
    const char* str;
} static_string;
static_string to_static_string(const char* str);
#endif