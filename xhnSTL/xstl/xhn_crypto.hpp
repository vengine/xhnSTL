//
//  xhn_crypto.h
//  VEngine
//
//  Created by 徐海宁 on 14/12/17.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#ifndef __VEngine__xhn_crypto__
#define __VEngine__xhn_crypto__

#include "sha256.h"
#include "xhn_string.hpp"
#include "xhn_void_vector.hpp"
namespace xhn
{
class sha256
{
private:
    struct _sha256 m_dataBase;
    euint8 m_digest[32];
public:
    sha256();
    ~sha256();
    void update(const string& str);
    void update(int i);
    void complete();
    string sha_string() const;
};
}

#endif /* defined(__VEngine__xhn_crypto__) */
