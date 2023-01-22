
#include "trackHeapUsage.h"

int main ()
{
    {
        auto test = std::make_shared<int>();
    }
    track_heap_usage::heap_usage::printUsage();
    return 0;
}