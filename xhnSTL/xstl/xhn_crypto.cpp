//
//  xhn_crypto.cpp
//  VEngine
//
//  Created by 徐海宁 on 14/12/17.
//  Copyright (c) 2014年 徐 海宁. All rights reserved.
//

#include "xhn_crypto.hpp"
#include "xhn_lambda.hpp"
xhn::sha256::sha256()
{
    Sha256_Init(&m_dataBase);
}
xhn::sha256::~sha256()
{}
void xhn::sha256::update(const string& str)
{
    Sha256_Update(&m_dataBase, (const euint8*)str.c_str(), str.size() * sizeof(*str.c_str()));
}
void xhn::sha256::update(int i)
{
    Sha256_Update(&m_dataBase, (const euint8*)&i, sizeof(i));
}
void xhn::sha256::complete()
{
    Sha256_Final(&m_dataBase, m_digest);
}
xhn::string xhn::sha256::sha_string() const
{
    char buffer[65];
    buffer[64] = 0x00;
    Lambda<char(euint8)> byteToCharProc([](euint8 _b){
        if (_b >= 0 && _b <= 9) {
            return '0' + _b;
        }
        else {
            return 'a' + (_b - 10);
        }
    });
    Lambda<void (euint8, char*)> byteToDoubleCharProc([&byteToCharProc](euint8 b, char* result){
        euint8 numberOfSixteen = b / 16;
        euint8 remainder = b % 16;
        result[0] = byteToCharProc(numberOfSixteen);
        result[1] = byteToCharProc(remainder);
    });
    for (esint i = 0; i < 32; i++) {
        char* doubleChar = &buffer[i * 2];
        byteToDoubleCharProc(m_digest[i], doubleChar);
    }
    return buffer;
}