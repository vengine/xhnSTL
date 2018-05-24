
#include "xhn_stacktrace.hpp"
#include <cxxabi.h>

namespace xhn {
    
string to_function_call(const char* str)
{
    string tmp0(str);
    euint pos = tmp0.find("_Z");
    if (string::npos != pos) {
        string func = tmp0.substr(pos, tmp0.size() - pos);
        euint spos = func.find(" ");
        if (string::npos != spos) {
            string tmp1 = func.substr(0, spos);
            string tmp2 = func.substr(spos, func.size() - spos);
            char* mbuf = (char*)NMalloc(1024 * 512);
            size_t len = 1024 * 512 - 1;
            int status = 0;
            abi::__cxa_demangle(tmp1.c_str(), mbuf, &len, &status);
            string tmp3 = tmp0.substr(0, pos);
            string ret = tmp3 + mbuf + tmp2;
            Mfree(mbuf);
            return ret;
        }
    }
    return tmp0;
}
    
template <typename T>
void cc()
{
    vector<T> result;
#if defined(__APPLE__)
    get_stacktrace(result);
#endif
    for (auto& s : result) {
        printf("%s\n", s.c_str());
    }
}

void bb()
{
    cc<string>();
}

void aa()
{
    bb();
}

void test_stacktrace()
{
    aa();
}
    
}
