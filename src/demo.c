#include <string.h>
// #include <sys/sysctl.h>

#include "Queue.h"
#include "ThreadPool.h"

#define JOB_NUM 10

void* f1(void* args) {
    int recasted_arg = *(int*)args;
    printf("Int: %d\n", recasted_arg);
    sleep(1);
    char* str = strdup("hello");
    return str;
}

void* f2(void* args) {
    const char* recasted_arg = *(const char**)args;
    printf("Str: %s\n", recasted_arg);
    return NULL;
}

void* f3(void) {
    printf("from c\n");
    return NULL;
}

void* f4(void) {
    printf("from d\n");
    return NULL;
}

int main(void) {
    size_t param_arr[JOB_NUM];
    char* param_arr_str[JOB_NUM];
    for (size_t i = 0; i < JOB_NUM; ++i) param_arr[i] = i + 1;
    for (size_t i = 0; i < JOB_NUM; ++i) {
        char* arr = (char*)malloc(sizeof(char) * 10);
        sprintf(arr, "%ld", i);
        param_arr_str[i] = arr;
    }

    /*int count;
    size_t size = sizeof(count);
    unsigned t_num =
        sysctlbyname("hw.ncpu", &count, &size, NULL, 0) ? 0 : count;*/
    Pool p = {.thread_size = 5};
    init_pool(&p);

    for (size_t i = 0; i < JOB_NUM; ++i) submit(&p, &f1, &param_arr[i]);
    for (size_t i = 0; i < JOB_NUM; ++i) submit(&p, &f2, &param_arr_str[i]);
    submit(&p, &f3, NULL);
    submit(&p, &f4, NULL);

    while (p.work_done != 2 * JOB_NUM + 2)
        ;

    while (atomic_load(&p.res.size)) {
        // debug("size: %d", p.res.size);
        Node res = deqeue(&p.res);
        if (res.val != NULL) {
            printf("output: %s\n", (char*)res.val);
            free((char*)res.val);
        }
    }

    close_pool(&p);

    for (size_t i = 0; i < JOB_NUM; ++i) free(param_arr_str[i]);

    return EXIT_SUCCESS;
}
