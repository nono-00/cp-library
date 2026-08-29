#pragma once

#include <cassert>

#ifndef DEBUG
#define DEBUG 0
#endif

namespace nono {

constexpr bool is_debug() {
    return DEBUG;
}

}  //  namespace nono

#define DEBUG_ASSERT(cond) \
    do { \
        if constexpr (nono::is_debug()) assert(cond); \
    } while (false)
