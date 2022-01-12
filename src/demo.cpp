#include <string.h>

#define JOB_NUM 10

#ifdef CPP_DEMO
#include "ThreadPool.hpp"
#elif defined C_DEMO
#include "ThreadPool.h"
#endif

void *f1(void *args) {
    int recasted_arg = *(int *)args;
    printf("Int: %d\n", recasted_arg);
    sleep(1);
    char *str = strdup("hello");
    return str;
}

void *f2(void *args) {
    const char *recasted_arg = *(const char **)args;
    printf("Str: %s\n", recasted_arg);
    sleep(1);
    return NULL;
}

void *f3(void) {
    printf("from c\n");
    return NULL;
}

void *f4(void) {
    printf("from d\n");
    return NULL;
}

int main(void) {
    size_t param_arr[JOB_NUM];
    char *param_arr_str[JOB_NUM];
    for (size_t i = 0; i < JOB_NUM; ++i)
        param_arr[i] = i + 1;
    for (size_t i = 0; i < JOB_NUM; ++i) {
        char *arr = (char *)c_malloc(sizeof(char) * JOB_NUM);
        sprintf(arr, "%ld", i);
        param_arr_str[i] = arr;
    }

#ifdef CPP_DEMO
    printf("C++ demo:\n");
    ThreadPool tp(4);
    for (size_t i = 0; i < JOB_NUM; ++i)
        tp.submitTask((const_T)&f1, &param_arr[i]);
    for (size_t i = 0; i < JOB_NUM; ++i)
        tp.submitTask((const_T)&f2, &param_arr_str[i]);
    tp.submitTask((const_T)&f3, NULL);
    tp.submitTask((const_T)&f4, NULL);

    tp.wait();

    while (tp.notEmpty()) {
        const_T res = tp.popResult();
        if (res) printf("output: %s\n", (char *)res);
        free((char *)res);
    }
#elif defined C_DEMO
    printf("C demo:\n");
    Pool tp;
    tp.thread_size = 6;
    init_pool(&tp);
    for (size_t i = 0; i < JOB_NUM; ++i)
        submit(&tp, (const_T)&f1, &param_arr[i]);
    for (size_t i = 0; i < JOB_NUM; ++i)
        submit(&tp, (const_T)&f2, &param_arr_str[i]);
    submit(&tp, (const_T)&f3, NULL);
    submit(&tp, (const_T)&f4, NULL);

    while (tp.work_remaining != 0)
        ;
    while (tp.res.size != 0) {
        Node res = deqeue(&tp.res);
        if (res.val) printf("output: %s\n", (char *)res.val);
        free((char *)res.val);
    }
    close_pool(&tp);
#endif

    for (size_t i = 0; i < JOB_NUM; ++i)
        free(param_arr_str[i]);

    return EXIT_SUCCESS;
}
