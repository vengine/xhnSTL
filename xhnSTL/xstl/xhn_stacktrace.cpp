
#include "xhn_stacktrace.hpp"

namespace xhn {

void bb()
{
    vector<string> result;
    get_stacktrace(result);
    for (auto& s : result) {
        printf("%s\n", s.c_str());
    }
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
