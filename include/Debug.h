#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <stdio.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s: %d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#endif
