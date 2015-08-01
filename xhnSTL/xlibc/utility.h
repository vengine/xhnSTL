#ifndef UTILITY_H
#define UTILITY_H
#include "common.h"
#include "etypes.h"
#ifdef __cplusplus
extern "C"
{
#endif
typedef struct _numeral_extract_status
{
    char buffer[256];
    int char_counter;
    int is_negatived;
    int has_decimal;
    int has_float_flag;
    int is_corrupted;
} numeral_extract_status;
void NES_init(numeral_extract_status* _self);
void NES_add_char(numeral_extract_status* _self, char c);
void NES_reset(numeral_extract_status* _self);
bool to_int(const char* str, int* result);
bool to_double(const char* str, double* result);
#ifdef __cplusplus
}
#endif
#endif