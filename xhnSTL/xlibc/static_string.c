#include "static_string.h"
#include "list.h"
#include "spin_lock.h"
#include "emem.h"
#include "hash.h"

#define ListFunc(s) StringList_##s
#include "container.h"
#include "list_base.h"

#define STRING_HASH_BUCKET_COUNT (1<<15)
#define STRING_HASH_MASK         (STRING_HASH_BUCKET_COUNT - 1)

typedef struct
{
    List string_list;
    ELock lock;
} string_hash_bucket;

typedef string_hash_bucket* StringHashBucket;

static void* StringHashAlloc(void* self, euint size)
{
    return Ealloc(size);
}
static void StringHashFree(void* self, void* ptr)
{
    Efree(ptr);
}

static native_memory_allocator g_StringHashAllocator =
{
    StringHashAlloc,
    StringHashFree,
    StringHashAlloc,
    StringHashFree,
};

void StringHashBucket_Init(StringHashBucket self, native_memory_allocator* alloc)
{
    self->string_list = StringList_new(Vptr, alloc);
#ifdef __APPLE__
#ifndef OSSPINLOCK_DEPRECATED
    self->lock = 0;
#else
    self->lock = OS_UNFAIR_LOCK_INIT;
#endif
#else
    self->lock = 0;
#endif
}

void StringHashBucket_Dest(StringHashBucket self)
{
    Iterator iter = StringList_begin(self->string_list);
    while (iter) {
        var v = StringList_get_value(iter);
        Mfree(v.vptr_var);
        iter = StringList_next(iter);
    }
}

const char* StringHashBucket_RefreshString(StringHashBucket self, const char* str, euint len)
{
    Iterator iter = StringList_begin(self->string_list);
    while (iter) {
        var v = StringList_get_value(iter);
        const char* s = (const char*)v.vptr_var;
        if (strcmp(s, str) == 0) {
            return s;
        }
        iter = StringList_next(iter);
    }
    var v;
    v.vptr_var = NMalloc(len + 1);
    memcpy(v.vptr_var, str, len + 1);
    StringList_push_back(self->string_list, v);
    return (const char*)v.vptr_var;
}

typedef struct
{
    string_hash_bucket buckets[STRING_HASH_BUCKET_COUNT];
} string_hash_set;

typedef string_hash_set* StringHashSet;

void StringHashSet_Init(StringHashSet self)
{
    for (int i = 0; i < STRING_HASH_BUCKET_COUNT; i++) {
        StringHashBucket_Init(&self->buckets[i], &g_StringHashAllocator);
    }
}

void StringHashSet_Dest(StringHashSet self)
{
    for (int i = 0; i < STRING_HASH_BUCKET_COUNT; i++) {
        StringHashBucket_Dest(&self->buckets[i]);
    }
}

const char* StringHashSet_RefreshString(StringHashSet self, const char* str)
{
    euint len = strlen(str);
    euint32 hash = calc_hashnr(str, len);
    StringHashBucket bucket = &self->buckets[hash & STRING_HASH_MASK];
    const char* ret = NULL;
    ELock_lock(&bucket->lock);
    ret = StringHashBucket_RefreshString(bucket, str, len);
    ELock_unlock(&bucket->lock);
    return ret;
}
#ifdef __APPLE__
#ifndef OSSPINLOCK_DEPRECATED
static ELock s_StringHashSetLock = 0;
#else
static ELock s_StringHashSetLock = OS_UNFAIR_LOCK_INIT;
#endif
#else
static ELock s_StringHashSetLock = 0;
#endif
static StringHashSet s_StringHashSet = NULL;

static_string to_static_string(const char* str)
{
    if (!s_StringHashSet) {
        ELock_lock(&s_StringHashSetLock);
        if (!s_StringHashSet) {
            s_StringHashSet = (StringHashSet)NMalloc(sizeof(string_hash_set));
            StringHashSet_Init(s_StringHashSet);
        }
        ELock_unlock(&s_StringHashSetLock);
    }
    static_string ret = {StringHashSet_RefreshString(s_StringHashSet, str)};
    return ret;
}
