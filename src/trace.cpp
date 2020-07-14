
#include "trace.h"

void r_assert(const char* assertion, const char* file, int line)
{
    ferr("Assertion failed: '{}' @{}:{}", assertion, file, line);

    int x = line / (line + 1);
    x = 1/x;
    *static_cast<volatile int*>(nullptr) = x;
    std::terminate();
}
