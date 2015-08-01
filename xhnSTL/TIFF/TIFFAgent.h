//
//  TIFFFileAgent.h
//  TIFF
//
//  Created by 徐海宁 on 1/11/15.
//  Copyright (c) 2015 徐 海宁. All rights reserved.
//

#ifndef __TIFF__TIFFFileAgent__
#define __TIFF__TIFFFileAgent__

#include <stdio.h>
#include "common.h"
#include "etypes.h"
#include "xhn_void_vector.hpp"

typedef struct tiff TIFF;
class TIFFAgent
{
public:
    static TIFF* Open(TIFFAgent* agent);
    static void Close(TIFF* tif);
    static void AddRGBA8Page(TIFF* tif,
                             euint32 pageIndex,
                             euint32 numberOfPages,
                             euint8* pixels,
                             euint32 width,
                             euint32 height);
    static void AddRGBA16Page(TIFF* tif,
                              euint32 pageIndex,
                              euint32 numberOfPages,
                              euint8* pixels,
                              euint32 width,
                              euint32 height);
public:
    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual euint64 Read(void* buffer, euint64 size) = 0;
    virtual euint64 Write(const void* buffer, euint64 size) = 0;
    virtual euint64 Seek(euint64 offset, int where) = 0;
    virtual euint64 Size() = 0;
};

class TIFFMemoryAgent : public TIFFAgent
{
public:
    euint64 m_offset;
    xhn::void_vector m_memory;
public:
    TIFFMemoryAgent()
    : m_offset(0)
    {}
public:
    virtual void Open();
    virtual void Close();
    virtual euint64 Read(void* buffer, euint64 size);
    virtual euint64 Write(const void* buffer, euint64 size);
    virtual euint64 Seek(euint64 offset, int where);
    virtual euint64 Size();
    inline const xhn::void_vector& GetMemory() const {
        return m_memory;
    }
    inline void Clear() {
        m_offset = 0;
        m_memory.clear();
    }
};
#endif /* defined(__TIFF__TIFFFileAgent__) */
