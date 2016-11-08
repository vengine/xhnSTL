#ifndef ETYPES_H
#define ETYPES_H
#include <stdio.h>
#include <stdlib.h>
///#include <memory.h>
#include <string.h>

#ifndef __cplusplus
#ifndef bool
#define bool char
#endif
#define true  1
#define false 0
#endif

typedef unsigned int euint32;
typedef unsigned long long euint64;
typedef long long esint64;

typedef char esint8;
typedef unsigned char euint8;
typedef unsigned short euint16;
typedef unsigned char* euptr8;
typedef euint32* euptr32;
typedef euint64* euptr64;

typedef const char* EString;

typedef int esint32;
typedef short esint16;
#ifdef _WIN64
typedef euint64 euint;
typedef esint64 esint;
#define MAX_EUINT 0xffffffffffffffff
#else
typedef euint32 euint;
typedef esint32 esint;
#define MAX_EUINT 0xffffffff
#endif
typedef void* vptr;
typedef euint pptr;
typedef esint* sptr;
typedef euint* uptr;

#define MAX_EUINT64 0xffffffffffffffff
#define INVALID_UNSIGNED_INTEGER_64 ((euint64)-1)
#define INVALID_UNSIGNED_INTEGER ((euint)-1)

typedef enum _etype
{
    Sint8,
    Sint16,
    Sint32,
    Sint64,
    Uint8,
    Uint16,
    Uint32,
    Uint64,
    Vptr,
    String,
}etype;

typedef union
{
    esint8 sint8_var;
    esint16 sint16_var;
    esint32 sint32_var;
    esint64 sint64_var;
    euint8 uint8_var;
    euint16 uint16_var;
    euint32 uint32_var;
    euint64 uint64_var;
    vptr vptr_var;
    EString str_var;
} var;

typedef euint ref_ptr;

struct _tree;
typedef struct _tree* Tree;

typedef void* (*MALLOC)(euint);
typedef void (*MFREE)(void*);

typedef int (*KEY_COMPARE)(vptr, vptr);

typedef bool (*ShowDialog)(void);
typedef void (*HideDialog)(void);

typedef void (*SwitchFileCallback)(const wchar_t*);

typedef struct _refer_info
{
	const char* file_name;
	euint32 line;
    euint32 test_mark;
} refer_info;
typedef void (*FnCheckMemory)(void* checker, void* memory, refer_info* info);

#define _32BIT_PIXEL_SIZE_ 4

#endif