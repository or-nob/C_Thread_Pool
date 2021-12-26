#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t thread_size) {
    this->pool.thread_size = thread_size;
    this->pool.thread_array = NULL;

    init_pool(&this->pool);
}

ThreadPool::~ThreadPool() { close_pool(&this->pool); }

void ThreadPool::submitTask(const_T f, T params) { submit(&this->pool, f, params); }

void ThreadPool::wait() {
    while (this->pool.work_remaining != 0)
        ;
}

const_T ThreadPool::popResult() {
    Node res = deqeue(&this->pool.res);
    return res.val;
}

bool ThreadPool::notEmpty() { return atomic_load(&this->pool.res.size); }
