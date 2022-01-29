#ifndef __ERRNO_H__
#define __ERRNO_H__

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define write_err_and_exit()                                                                       \
    do {                                                                                           \
        fprintf(stderr, "%s\n", strerror(errno));                                                  \
        exit(1);                                                                                   \
    } while (0)

#endif
