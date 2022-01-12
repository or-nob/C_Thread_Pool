#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <pthread.h>
#ifndef __cplusplus
#include <stdatomic.h>
#else
#include <atomic>
#define atomic_bool std::atomic_bool
#define atomic_int std::atomic_int
#endif

#include "Debug.h"
#include "Helper.h"

typedef const void *const_T;

typedef struct Node {
    const_T val;
    struct Node *next;
    struct Node *prev;
} Node;

typedef struct {
    atomic_int size;
    Node *head;
    Node *tail;
} Queue;

#ifdef __cplusplus
extern "C" {
#endif
void init_queue(Queue *q);
void enqueue(Queue *q, const_T val);
Node deqeue(Queue *q);
void free_queue(Queue *q);

#ifdef __cplusplus
}
#endif

#endif
