#include "utility.h"
void NES_init(numeral_extract_status* _self)
{
    _self->char_counter = 0;
    _self->is_negatived = 0;
    _self->has_decimal = 0;
    _self->has_float_flag = 0;
    _self->is_corrupted = 0;
}
void NES_reset(numeral_extract_status* _self)
{
    _self->char_counter = 0;
    _self->is_negatived = 0;
    _self->has_decimal = 0;
    _self->has_float_flag = 0;
    _self->is_corrupted = 0;
}
void NES_add_char(numeral_extract_status* _self, char c)
{
    if (_self->is_corrupted == 0) {
        if (_self->has_float_flag == 1) {
            _self->is_corrupted = 1;
        }
        else if (_self->char_counter == 255) {
            _self->is_corrupted = 1;
        }
        else if (c == 'f') {
            if (_self->has_float_flag == 0 && _self->char_counter > 0) {
                _self->buffer[_self->char_counter++] = c;
                _self->has_float_flag = 1;
            }
            else {
                _self->is_corrupted = 1;
            }
        }
        else if (c == '.') {
            if (_self->has_decimal == 0) {
                _self->buffer[_self->char_counter++] = c;
                _self->has_decimal = 1;
            }
            else {
                _self->is_corrupted = 1;
            }
        }
        else if (c == '-') {
            if (_self->char_counter == 0) {
                _self->buffer[_self->char_counter++] = c;
                _self->is_negatived = 1;
            }
            else {
                _self->is_corrupted = 1;
            }
        }
        else if (c >= '0' && c <= '9') {
            _self->buffer[_self->char_counter++] = c;
        }
        else {
            _self->is_corrupted = 1;
        }
    }
}

bool to_int(const char* str, int* result)
{
    int count = 0;
    bool isnegatived = false;
    if (str[count] == '-') {
        isnegatived = true;
        count++;
    }
    while (str[count]) {
        if (str[count] == '.') {
            return false;
        }
        else if (str[count] < '0' ||
                 str[count] > '9') {
            return false;
        }
        count++;
    }
    /// 计算整数部分
    int integer = 0;
    int iweight = 1;
    int n = count;
    if (!isnegatived) {
        while (n > 0) {
            char c = str[n - 1];
            integer += ((c - '0') * iweight);
            iweight *= 10;
            n--;
        }
    }
    else {
        while (n > 1) {
            char c = str[n - 1];
            integer += ((c - '0') * iweight);
            iweight *= 10;
            n--;
        }
    }
    if (!isnegatived)
        *result = integer;
    else
        *result = -integer;
    return true;
}
bool to_double(const char* str, double* result)
{
    int count = 0;
    int dot = -1;
    bool isnegatived = false;
    /// 判断是否是负数
    if (str[0] && str[0] == '-') {
        isnegatived = true;
        str++;
    }
    while (str[count] && str[count] != 'f') {
        if (str[count] == '.') {
            if (dot != -1)
                return false;
            dot = count;
        }
        else if (str[count] < '0' ||
                 str[count] > '9') {
            return false;
        }
        count++;
    }
    if (dot == -1) {
        /// 只有整数部分
        int integer = 0;
        int iweight = 1;
        int n = count;
        while (n > 0) {
            char c = str[n - 1];
            integer += ((c - '0') * iweight);
            iweight *= 10;
            n--;
        }
        if (isnegatived) {
            *result = -(double)integer;
        }
        else {
            *result = (double)integer;
        }
        return true;
    }
    else {
        /// 计算整数部分
        int integer = 0;
        int iweight = 1;
        int n = dot;
        while (n > 0) {
            char c = str[n - 1];
            integer += ((c - '0') * iweight);
            iweight *= 10;
            n--;
        }
        /// 计算小数部分
        n = dot + 1;
        double flt = 0.0;
        double fweight = 0.1;
        while (n < count) {
            char c = str[n];
            flt += ((double)(c - '0') * fweight);
            fweight *= 0.1;
            n++;
        }
        if (isnegatived) {
            *result = -((double)integer + flt);
        }
        else {
            *result = (double)integer + flt;
        }
        return true;
    }
}