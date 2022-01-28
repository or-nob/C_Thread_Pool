#include "ThreadPool.h"

void init_pool(Pool *p) {
    init_queue(&p->q);
    init_queue(&p->res);

    atomic_store(&p->pred, 0);
    atomic_store(&p->work_remaining, 0);
    atomic_store(&p->busy_threads, 0);
    p->number_of_threads = 0;

    int8_t ret = -1;
    ret = pthread_mutex_init(&p->lock, NULL);
    assert(ret == 0);
    ret = pthread_mutex_init(&p->res_lock, NULL);
    assert(ret == 0);
    ret = pthread_cond_init(&p->cond, NULL);
    assert(ret == 0);

    init_queue(&p->thread_queue);
}

void submit(Pool *p, const_T f, T params) {
    pthread_t *t = NULL;
    int8_t ret = -1;
    if (p->number_of_threads < p->thread_size) {
        t = (pthread_t *)c_malloc(sizeof(pthread_t));
        p->number_of_threads++;
        ret = pthread_create(t, NULL, run, p);
        assert(ret == 0);
        enqueue(&p->thread_queue, t);
        // atomic_fetch_add(&p->busy_threads, 1);
    }
    Function *func = (Function *)c_malloc(sizeof(Function));
    func->f = f;
    func->params = params;
    ret = pthread_mutex_lock(&p->lock);
    assert(ret == 0);
    enqueue(&p->q, func);
    ret = pthread_cond_signal(&p->cond);
    assert(ret == 0);
    ret = pthread_mutex_unlock(&p->lock);
    assert(ret == 0);
    atomic_fetch_add(&p->work_remaining, 1);
}

T run(T arg) {
    Pool *p = (Pool *)arg;
    while (1) {
        const_T r;
        int8_t ret = -1;
        ret = pthread_mutex_lock(&p->lock);
        assert(ret == 0);
        while (!(atomic_load_explicit(&p->q.size, memory_order_relaxed)) &&
               !atomic_load_explicit(&p->pred, memory_order_relaxed)) {
            ret = pthread_cond_wait(&p->cond, &p->lock);
            assert(ret == 0);
        }

        if (atomic_load_explicit(&p->pred, memory_order_relaxed)) {
            ret = pthread_mutex_unlock(&p->lock);
            assert(ret == 0);
            goto end;
        }

        r = deqeue(&p->q);
        assert(r != NULL);
        ret = pthread_mutex_unlock(&p->lock);
        assert(ret == 0);
        atomic_fetch_add(&p->busy_threads, 1);
        // debug("here %d", atomic_load(&p->busy_threads));

        Function *f_ptr = (Function *)r;
        func func = f_ptr->f;
        T res = func(f_ptr->params);

        free(f_ptr);

        ret = pthread_mutex_lock(&p->res_lock);
        assert(ret == 0);
        enqueue(&p->res, res);
        ret = pthread_mutex_unlock(&p->res_lock);
        assert(ret == 0);
        atomic_fetch_sub(&p->work_remaining, 1);
        atomic_fetch_sub(&p->busy_threads, 1);
    }

end:
    return NULL;
}

void join_pool(Pool *p) {
    int8_t ret = -1;
    while (atomic_load(&p->thread_queue.size)) {
        const_T res = deqeue(&p->thread_queue);
        pthread_t *t = (pthread_t *)res;
        int ret = pthread_join(*t, NULL);
        assert(ret == 0);
        free(t);
    }
}

void close_pool(Pool *p) {
    int8_t ret = -1;
    ret = pthread_mutex_lock(&p->lock);
    assert(ret == 0);
    atomic_store(&p->pred, 1);
    ret = pthread_cond_broadcast(&p->cond);
    assert(ret == 0);
    ret = pthread_mutex_unlock(&p->lock);
    assert(ret == 0);

    join_pool(p);
    free_pool(p);
}

void free_pool(Pool *p) {
    free_queue(&p->thread_queue);
    free_queue(&p->res);
    free_queue(&p->q);
}
