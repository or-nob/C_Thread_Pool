#include "Helper.h"

T c_malloc(size_t size) {
    T addr = malloc(size);
    assert(addr != NULL);
    return addr;
}
