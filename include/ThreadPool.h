#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <sys/types.h>
#include <unistd.h>

#include "Queue.h"
#include "error_write.h"

typedef T (*func)(T);

typedef struct {
    Queue thread_queue;
    size_t thread_size;
    size_t created_threads_cnt;
    pthread_mutex_t lock;
    pthread_mutex_t res_lock;
    pthread_mutex_t cond_lock;
    pthread_cond_t cond;
    atomic_bool pred;
    Queue q;
    Queue res;
    size_t work_remaining;
    size_t busy_threads;
    size_t running_thread_cnt;
} Pool;

typedef struct {
    const_T f;
    T params;
} Function;

#ifdef __cplusplus
extern "C" {
#endif

void init_pool(Pool *p);
void submit(Pool *p, const_T f, T params);
static T run(T arg);
void join_pool(Pool *p);
void wait_pool(Pool *p);
void close_pool(Pool *p);
void free_pool(Pool *p);

#ifdef __cplusplus
}
#endif

#endif
