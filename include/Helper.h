#ifndef HELPER_HPP
#define HELPER_HPP

#include <assert.h>
#include <stdlib.h>
typedef void* T;

#ifdef __cplusplus
extern "C" {
#endif

T c_malloc(size_t size);

#ifdef __cplusplus
}
#endif

#endif
