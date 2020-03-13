/**
 * xuhaining's Standard Template Library - version 1.0
 * --------------------------------------------------------
 * Copyright (C) 2011-2013, by Xu Haining (xhnsworks@gmail.com)
 * Download new versions at https://github.com/vengine/XGC
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 */

#ifndef SHA256_H
#define SHA256_H

#include "common.h"
#include "etypes.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define SHA256_DIGEST_SIZE 32

struct _sha256
{
  euint32 state[8];
  euint64 count;
  euint8 buffer[64];
};
typedef struct _sha256* Sha256;
XHN_EXPORT void Sha256_Init(Sha256 _self);
XHN_EXPORT void Sha256_Update(Sha256 _self, const euint8 *data, euint size);
XHN_EXPORT void Sha256_Final(Sha256 _self, euint8 *digest);

#ifdef __cplusplus
}
#endif

#endif
