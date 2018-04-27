/*
 * Copyright (c) 2011-2018, 徐海宁
 * All rights reserved.
 *
 * 文件名称：
 * 文件描述：
 *
 * 当前版本：0.9.3
 * 作    者：徐海宁
 */

#ifndef COLLADATOOL_HPP
#define COLLADATOOL_HPP

namespace VEngine {

class ColladaTool
{
public:
    STD::string m_colladaPath;
    STD::string m_colladaContent;
public:
    void Open(const char* colladaPath)
    {
        FILE* file = fopen(colladaPath, "r");
        if (file) {
            fseek(file, 0, SEEK_END);
            size_t length = ftell(file);
            fseek(file, 0, SEEK_SET);
            char* buf = (char*)malloc(length + 1);
            fread(buf, 1, length, file);
            buf[length] = 0x00;
            m_colladaContent = buf;
            fclose(file);
            free(buf);
            m_colladaPath = colladaPath;
        }
    }
    void Save()
    {
        FILE* file = fopen(m_colladaPath.c_str(), "w");
        if (file) {
            fwrite(m_colladaContent.c_str(), 1, m_colladaContent.size(), file);
            fclose(file);
        }
    }
    void ResetTexturesDirectory(const STD::vector<STD::pair<STD::string, STD::string>>& resetDirVec)
    {
        for (auto& p : resetDirVec) {
            STD::string oriDir = p.first;
            STD::string newDir = p.second;
            size_t loc = 0;
            size_t pos = m_colladaContent.find(oriDir, loc);
            while (STD::string::npos != pos) {
                STD::string tmp0 = m_colladaContent.substr(0, pos);
                STD::string tmp1 = m_colladaContent.substr(pos + oriDir.size(), m_colladaContent.size() - pos - oriDir.size());
                m_colladaContent = tmp0 + newDir + tmp1;
                loc = pos + newDir.size();
                pos = m_colladaContent.find(oriDir, loc);
            }
        }
    }
};
    
}

#endif
