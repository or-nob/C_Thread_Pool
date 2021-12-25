#ifndef __THREADPOOL_HPP__
#define __THREADPOOL_HPP__

#include "ThreadPool.h"

class ThreadPool {
    Pool pool;

public:
    ThreadPool(size_t thread_size);
    ~ThreadPool();
    void submitTask(const_T f, T params);
    void wait();
    const_T popResult();
    bool notEmpty();
};

#endif
