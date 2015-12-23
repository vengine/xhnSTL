#include "xhn_gc_string.h"

xhn::GCUtf8String::GCUtf8String()
{
    m_memory  = Alloc(sizeof(char) * 1);
    char* tmp = (char*)m_memory->Get();
    m_string = (const char*)m_memory->Get();
    tmp[0] = 0x00;
    m_size = 0;
    gc->Attach(gcNode, m_memory->gcNode);
}
xhn::GCUtf8String::GCUtf8String(const Utf8& str)
{
    const string& s = (string)str;
    m_memory  = Alloc(sizeof(char) * (s.size() + 1));
    char* tmp = (char*)m_memory->Get();
    m_string = (const char*)m_memory->Get();
    memcpy(tmp, s.c_str(), sizeof(char) * s.size());
    tmp[s.size()] = 0x00;
    m_size = s.size();
    gc->Attach(gcNode, m_memory->gcNode);
}