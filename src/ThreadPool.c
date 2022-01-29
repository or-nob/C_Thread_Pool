#include "ThreadPool.h"

void init_pool(Pool *p) {
    init_queue(&p->q);
    init_queue(&p->res);

    atomic_store(&p->pred, 0);
    p->work_remaining = 0;
    p->busy_threads = 0;
    p->running_thread_cnt = 0;
    p->created_threads_cnt = 0;

    int8_t ret = -1;
    ret = pthread_mutex_init(&p->lock, NULL);
    if (ret < 0) write_err_and_exit();
    ret = pthread_mutex_init(&p->res_lock, NULL);
    if (ret < 0) write_err_and_exit();
    ret = pthread_mutex_init(&p->cond_lock, NULL);
    if (ret < 0) write_err_and_exit();
    ret = pthread_cond_init(&p->cond, NULL);
    if (ret < 0) write_err_and_exit();

    init_queue(&p->thread_queue);
}

void submit(Pool *p, const_T f, T params) {
    pthread_t *t = NULL;
    int8_t ret = -1;

    if (p->created_threads_cnt < p->thread_size) {
        int8_t r = 1;
        ret = pthread_mutex_lock(&p->cond_lock);
        if (ret < 0) write_err_and_exit();
        r = ((p->running_thread_cnt - p->busy_threads) < p->work_remaining) ||
            !p->running_thread_cnt;
        ret = pthread_mutex_unlock(&p->cond_lock);
        if (ret < 0) write_err_and_exit();
        if (r) {
            t = (pthread_t *)c_malloc(sizeof(pthread_t));
            p->created_threads_cnt++;
            ret = pthread_create(t, NULL, run, p);
            if (ret < 0) write_err_and_exit();
            enqueue(&p->thread_queue, t);
        }
    }

    Function *func = (Function *)c_malloc(sizeof(Function));
    func->f = f;
    func->params = params;
    ret = pthread_mutex_lock(&p->lock);
    if (ret < 0) write_err_and_exit();
    enqueue(&p->q, func);
    ret = pthread_cond_signal(&p->cond);
    if (ret < 0) write_err_and_exit();
    ret = pthread_mutex_unlock(&p->lock);
    if (ret < 0) write_err_and_exit();
    ret = pthread_mutex_lock(&p->cond_lock);
    if (ret < 0) write_err_and_exit();
    p->work_remaining++;
    ret = pthread_mutex_unlock(&p->cond_lock);
    if (ret < 0) write_err_and_exit();
}

T run(T arg) {
    Pool *p = (Pool *)arg;
    int8_t ret = -1;
    ret = pthread_mutex_lock(&p->cond_lock);
    if (ret < 0) write_err_and_exit();
    p->running_thread_cnt++;
    ret = pthread_mutex_unlock(&p->cond_lock);
    if (ret < 0) write_err_and_exit();
    while (1) {
        const_T r;
        ret = pthread_mutex_lock(&p->lock);
        if (ret < 0) write_err_and_exit();
        while (!(atomic_load_explicit(&p->q.size, memory_order_relaxed)) &&
               !atomic_load_explicit(&p->pred, memory_order_relaxed)) {
            ret = pthread_cond_wait(&p->cond, &p->lock);
            if (ret < 0) write_err_and_exit();
        }

        if (atomic_load_explicit(&p->pred, memory_order_relaxed)) {
            ret = pthread_mutex_unlock(&p->lock);
            if (ret < 0) write_err_and_exit();
            goto end;
        }

        r = deqeue(&p->q);
        assert(r != NULL);
        ret = pthread_mutex_unlock(&p->lock);
        if (ret < 0) write_err_and_exit();

        ret = pthread_mutex_lock(&p->cond_lock);
        if (ret < 0) write_err_and_exit();
        p->busy_threads++;
        ret = pthread_mutex_unlock(&p->cond_lock);
        if (ret < 0) write_err_and_exit();

        Function *f_ptr = (Function *)r;
        func func = f_ptr->f;
        T res = func(f_ptr->params);

        free(f_ptr);

        ret = pthread_mutex_lock(&p->res_lock);
        if (ret < 0) write_err_and_exit();
        enqueue(&p->res, res);
        ret = pthread_mutex_unlock(&p->res_lock);
        if (ret < 0) write_err_and_exit();

        ret = pthread_mutex_lock(&p->cond_lock);
        if (ret < 0) write_err_and_exit();
        p->work_remaining--;
        p->busy_threads--;
        ret = pthread_mutex_unlock(&p->cond_lock);
        if (ret < 0) write_err_and_exit();
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
        if (ret < 0) write_err_and_exit();
        free(t);
    }
}

void close_pool(Pool *p) {
    int8_t ret = -1;
    ret = pthread_mutex_lock(&p->lock);
    if (ret < 0) write_err_and_exit();
    atomic_store(&p->pred, 1);
    ret = pthread_cond_broadcast(&p->cond);
    if (ret < 0) write_err_and_exit();
    ret = pthread_mutex_unlock(&p->lock);
    if (ret < 0) write_err_and_exit();

    join_pool(p);
    free_pool(p);
}

void free_pool(Pool *p) {
    free_queue(&p->thread_queue);
    free_queue(&p->res);
    free_queue(&p->q);
}

void wait_pool(Pool *p) {
    int8_t ret = -1;
    for (;;) {
        ret = pthread_mutex_lock(&p->cond_lock);
        if (ret < 0) write_err_and_exit();
        if (!p->work_remaining) {
            ret = pthread_mutex_unlock(&p->cond_lock);
            if (ret < 0) write_err_and_exit();
            break;
        }
        ret = pthread_mutex_unlock(&p->cond_lock);
        if (ret < 0) write_err_and_exit();
    }
}
