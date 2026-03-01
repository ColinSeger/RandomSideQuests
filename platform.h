#pragma once
#include <stdint.h>

struct Timer{
    uint64_t ns;
};

#ifdef WIN32
#include "windows_platform.h"
#else
#include "linux_platform.h"
#endif

static inline struct Timer profile_start();

static inline void profile_end(struct Timer start);
