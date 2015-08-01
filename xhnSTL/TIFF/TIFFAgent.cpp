//
//  TIFFFileAgent.cpp
//  TIFF
//
//  Created by 徐海宁 on 1/11/15.
//  Copyright (c) 2015 徐 海宁. All rights reserved.
//

#include "TIFFAgent.h"
#include "tiffio.h"

tsize_t tiff_Read(thandle_t st,tdata_t buffer,tsize_t size)
{
    TIFFAgent* agent = (TIFFAgent*)st;
    return agent->Read(buffer, size);
};

tsize_t tiff_Write(thandle_t st,tdata_t buffer,tsize_t size)
{
    TIFFAgent* agent = (TIFFAgent*)st;
    return agent->Write(buffer, size);
};

int tiff_Close(thandle_t st)
{
    TIFFAgent* agent = (TIFFAgent*)st;
    agent->Close();
    return 0;
};

toff_t tiff_Seek(thandle_t st,toff_t pos, int whence)
{
    if (pos == 0xFFFFFFFF)
        return 0xFFFFFFFF;
    TIFFAgent* agent = (TIFFAgent*)st;
    return agent->Seek(pos, whence);
};

toff_t tiff_Size(thandle_t st)
{
    TIFFAgent* agent = (TIFFAgent*)st;
    return agent->Size();
};

int tiff_Map(thandle_t, tdata_t*, toff_t*)
{
    return 0;
};

void tiff_Unmap(thandle_t, tdata_t, toff_t)
{
    return;
};

TIFF* TIFFAgent::Open(TIFFAgent* agent)
{
    return TIFFClientOpen("memory", "w", agent, tiff_Read, tiff_Write, tiff_Seek, tiff_Close, tiff_Size, tiff_Map, tiff_Unmap);
}
void TIFFAgent::Close(TIFF* tif)
{
    TIFFClose(tif);
}
void TIFFAgent::AddRGBA8Page(TIFF* tif,
                             euint32 pageIndex,
                             euint32 numberOfPages,
                             euint8* pixels,
                             euint32 width,
                             euint32 height)
{
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 4);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    TIFFSetField(tif, TIFFTAG_XRESOLUTION, 72);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, 72);
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
    
    /* We are writing single page of the multipage file */
    TIFFSetField(tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
    /* Set the page number */
    TIFFSetField(tif, TIFFTAG_PAGENUMBER, pageIndex, numberOfPages);
    
    for (euint32 i = 0; i < height; i++)
        TIFFWriteScanline(tif, &pixels[i * width * 4], i, 0);
    
    TIFFWriteDirectory(tif);
}
void TIFFAgent::AddRGBA16Page(TIFF* tif,
                              euint32 pageIndex,
                              euint32 numberOfPages,
                              euint8* pixels,
                              euint32 width,
                              euint32 height)
{
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, 4);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_BOTLEFT);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    
    TIFFSetField(tif, TIFFTAG_XRESOLUTION, 72);
    TIFFSetField(tif, TIFFTAG_YRESOLUTION, 72);
    TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
    
    /* We are writing single page of the multipage file */
    TIFFSetField(tif, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
    /* Set the page number */
    TIFFSetField(tif, TIFFTAG_PAGENUMBER, pageIndex, numberOfPages);
    
    for (euint32 i = 0; i < height; i++)
        TIFFWriteScanline(tif, &pixels[i * width * 4 * 2], i, 0);
    
    TIFFWriteDirectory(tif);
}

void TIFFMemoryAgent::Open()
{
    m_memory.convert<xhn::void_vector::FCharProc>(1);
    m_memory.reserve(1024 * 1024);
}
void TIFFMemoryAgent::Close()
{
    ///
}
euint64 TIFFMemoryAgent::Read(void* buffer, euint64 size)
{
    if (m_offset + size > m_memory.size()) {
        size = m_memory.size() - m_offset;
    }
    if (size) {
        memcpy(buffer, (char*)m_memory.get() + m_offset, size);
    }
    m_offset += size;
    return size;
}
euint64 TIFFMemoryAgent::Write(const void* buffer, euint64 size)
{
    if (m_offset + size > m_memory.size()) {
        m_memory.resize(m_offset + size);
    }
    if (size) {
        char* mem = (char*)m_memory.get();
        memcpy(mem + m_offset, buffer, size);
    }
    m_offset += size;
    return size;
}
euint64 TIFFMemoryAgent::Seek(euint64 offset, int where)
{
    if (SEEK_SET == where) {
        m_offset = offset;
    }
    else if (SEEK_CUR == where) {
        m_offset += offset;
    }
    else {
        m_offset = m_memory.size() - offset;
    }
    return m_offset;
}
euint64 TIFFMemoryAgent::Size()
{
    return m_memory.size();
}