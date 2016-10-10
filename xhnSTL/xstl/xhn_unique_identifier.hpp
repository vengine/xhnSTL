//
//  xhn_unique_identifier.hpp
//  xhnSTL
//
//  Created by 徐海宁 on 16/3/4.
//  Copyright © 2016年 徐 海宁. All rights reserved.
//

#ifndef xhn_unique_identifier_hpp
#define xhn_unique_identifier_hpp

#include "common.h"
#include "etypes.h"
#include "xhn_string.hpp"

#ifdef __cplusplus

namespace xhn
{
    euint create_uid();
    string create_uuid_string();
}

#endif

#endif /* xhn_unique_identifier_hpp */
