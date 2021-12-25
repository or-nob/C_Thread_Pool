#ifdef __cplusplus
extern "C" {
#endif

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdlib.h>

#include "Debug.h"

typedef const void* const_T;
typedef void* T;

typedef struct Node {
    const_T val;
    T param;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct {
    atomic_int size;
    Node* head;
    Node* tail;
} Queue;

void init_queue(Queue* q);
void enqueue(Queue* q, const_T val, T param);
Node deqeue(Queue* q);
void free_queue(Queue* q);

#ifdef __cplusplus
}
#endif

#endif
