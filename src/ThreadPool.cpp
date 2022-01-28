#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t thread_size) {
    this->pool.thread_size = thread_size;
    init_pool(&this->pool);
}

ThreadPool::~ThreadPool() { close_pool(&this->pool); }

void ThreadPool::submitTask(const_T f, T params) { submit(&this->pool, f, params); }

void ThreadPool::wait() {
    while (atomic_load(&this->pool.work_remaining))
        ;
}

const_T ThreadPool::popResult() { return deqeue(&this->pool.res); }

bool ThreadPool::notEmpty() { return atomic_load(&this->pool.res.size); }
