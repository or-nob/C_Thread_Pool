#include "ThreadPool.h"

void init_pool(Pool* p) {
    p->thread_array = (pthread_t*)malloc(sizeof(pthread_t) * p->thread_size);
    assert(p->thread_array != NULL);
    atomic_store(&p->pred, 0);
    atomic_store(&p->work_done, 0);
    pthread_mutex_init(&p->lock, NULL);
    pthread_mutex_init(&p->res_lock, NULL);
    pthread_cond_init(&p->cond, NULL);
    for (size_t i = 0; i < p->thread_size; ++i)
        pthread_create(&p->thread_array[i], NULL, run, p);
}

void submit(Pool* p, const_T f, T params) {
    pthread_mutex_lock(&p->lock);
    enqueue(&p->q, f, params);
    pthread_cond_signal(&p->cond);
    pthread_mutex_unlock(&p->lock);
}

T run(T arg) {
    Pool* p = (Pool*)arg;
    while (1) {
        Node n;
        pthread_mutex_lock(&p->lock);
        while (!(atomic_load(&p->q.size)) && !atomic_load(&p->pred))
            pthread_cond_wait(&p->cond, &p->lock);

        if (atomic_load(&p->pred)) {
            pthread_mutex_unlock(&p->lock);
            goto end;
        }

        n = deqeue(&p->q);
        pthread_mutex_unlock(&p->lock);
        assert(n.val != NULL);
        func f = n.val;
        T res = f(n.param);
        pthread_mutex_lock(&p->res_lock);
        enqueue(&p->res, res, NULL);
        pthread_mutex_unlock(&p->res_lock);
        atomic_fetch_add(&p->work_done, 1);
    }

end:
    return NULL;
}

void join_pool(Pool* p) {
    for (size_t i = 0; i < p->thread_size; ++i)
        pthread_join(p->thread_array[i], NULL);
}

void close_pool(Pool* p) {
    pthread_mutex_lock(&p->lock);
    atomic_store(&p->pred, 1);
    pthread_cond_broadcast(&p->cond);
    pthread_mutex_unlock(&p->lock);

    join_pool(p);
    free_pool(p);
}

void free_pool(Pool* p) {
    free(p->thread_array);
    free_queue(&p->res);
    free_queue(&p->q);
}
