//
//  xhn_unique_identifier.cpp
//  xhnSTL
//
//  Created by 徐海宁 on 16/3/4.
//  Copyright © 2016年 徐 海宁. All rights reserved.
//

#include "xhn_unique_identifier.hpp"
#include "xhn_list.hpp"
#include "xhn_lock.hpp"
#include "xhn_set.hpp"
#include "xhn_utility.hpp"
#include "timer.h"

static xhn::SpinLock s_uid_lock;
static xhn::list<euint>* s_uid_list = nullptr;

euint fast_hashnr(euint64 timeStamp)
{
    euint nr=1, nr2=4;
    euint8* key = (euint8*)&timeStamp;
    euint length = sizeof(timeStamp);
    while (length--)
    {
        nr^= (((nr & 63)+nr2)*((euint) *key++))+ (nr << 8);
        nr2+=3;
    }
    
    return (nr & 0xfffffffffffffff0) | ((nr >> 16) & 0x000000000000000f);
}
euint xhn::create_uid()
{
    xhn::SpinLock::Instance inst = s_uid_lock.Lock();
    if (!s_uid_list) {
        s_uid_list = VNEW xhn::list<euint>();
    }
    if (s_uid_list->empty()) {
        xhn::set<euint> uidSet;
        TimeCheckpoint tc = TimeCheckpoint::Tick();
        for (euint i = 0; i < 1000; i++) {
            euint uid = fast_hashnr(tc.timeStamp);
            while (uidSet.find(uid) != uidSet.end()) {
                tc.timeStamp++;
                uid = fast_hashnr(tc.timeStamp);
            }
            uidSet.insert(uid);
            s_uid_list->push_back(uid);
        }
    }
    euint ret = s_uid_list->front();
    s_uid_list->pop_front();
    return ret;
}

#include <uuid/uuid.h>
xhn::string xhn::create_uuid_string()
{
    char mbuf[256];
    char* strbuf = mbuf;
    int remainder = 255;
    uuid_t uuid;
    uuid_generate(uuid);
    
    unsigned char *p = uuid;
    for (int i = 0; i < sizeof(uuid_t); i++, p++)
    {
        int len = snprintf(strbuf, remainder, "%02x", *p);
        strbuf += len;
        remainder -= len;
    }
    
    xhn::string ret(mbuf);
    return ret;
}
