#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <sys/types.h>
#include <unistd.h>

#include "Queue.h"

typedef void* (*func)(void*);

typedef struct {
    pthread_t* thread_array;
    size_t thread_size;
    pthread_mutex_t lock;
    pthread_mutex_t res_lock;
    pthread_cond_t cond;
    atomic_bool pred;
    Queue q;
    Queue res;
    atomic_int work_done;
} Pool;

void init_pool(Pool* p);
void submit(Pool* p, const_T f, T params);
static void* run(void* arg);
void join_pool(Pool* p);
void close_pool(Pool* p);
void free_pool(Pool* p);

#endif
