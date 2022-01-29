#include "Helper.h"
#include "error_write.h"

T c_malloc(size_t size) {
    T addr = malloc(size);
    if (!addr) write_err_and_exit();
    return addr;
}
