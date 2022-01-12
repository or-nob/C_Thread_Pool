#include "ThreadPool.h"

void init_pool(Pool *p) {
    init_queue(&p->q);
    init_queue(&p->res);

    atomic_store(&p->pred, 0);
    atomic_store(&p->work_remaining, 0);

    int8_t ret = -1;
    ret = pthread_mutex_init(&p->lock, NULL);
    assert(ret == 0);
    ret = pthread_mutex_init(&p->res_lock, NULL);
    assert(ret == 0);
    ret = pthread_cond_init(&p->cond, NULL);
    assert(ret == 0);

    p->thread_array = (pthread_t *)c_malloc(sizeof(pthread_t) * p->thread_size);

    for (size_t i = 0; i < p->thread_size; ++i) {
        ret = pthread_create(&p->thread_array[i], NULL, run, p);
        assert(ret == 0);
    }
}

void submit(Pool *p, const_T f, T params) {
    int8_t ret = -1;
    ret = pthread_mutex_lock(&p->lock);
    assert(ret == 0);
    Function *func = (Function *)c_malloc(sizeof(Function));
    func->f = f;
    func->params = params;
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
        Node n;
        int8_t ret = -1;
        ret = pthread_mutex_lock(&p->lock);
        assert(ret == 0);
        // debug("%p %p %d %d", p, &p->q, atomic_load(&p->q.size),
        //       atomic_load(&p->pred));
        while (!(atomic_load(&p->q.size)) && !atomic_load(&p->pred)) {
            ret = pthread_cond_wait(&p->cond, &p->lock);
            assert(ret == 0);
        }

        if (atomic_load(&p->pred)) {
            ret = pthread_mutex_unlock(&p->lock);
            assert(ret == 0);
            goto end;
        }

        n = deqeue(&p->q);
        assert(n.val != NULL);
        ret = pthread_mutex_unlock(&p->lock);
        assert(ret == 0);
        Function *f_ptr = (Function *)n.val;
        func func = f_ptr->f;
        T res = func(f_ptr->params);

        free(f_ptr);

        ret = pthread_mutex_lock(&p->res_lock);
        assert(ret == 0);
        enqueue(&p->res, res);
        ret = pthread_mutex_unlock(&p->res_lock);
        assert(ret == 0);
        atomic_fetch_sub(&p->work_remaining, 1);
    }

end:
    return NULL;
}

void join_pool(Pool *p) {
    int8_t ret = -1;
    for (size_t i = 0; i < p->thread_size; ++i) {
        ret = pthread_join(p->thread_array[i], NULL);
        assert(ret == 0);
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
    free(p->thread_array);
    free_queue(&p->res);
    free_queue(&p->q);
}
