#include "Queue.h"

void init_queue(Queue* q) {
    assert(q != NULL);
    q->head = NULL;
    atomic_store(&q->size, 0);
}

void enqueue(Queue* q, const_T val) {
    assert(q != NULL);
    Node* new_node = (Node*)malloc(sizeof(Node));
    assert(new_node != NULL);
    new_node->val = val;
    new_node->prev = NULL;
    new_node->next = NULL;
    if (!q->head) {
        q->head = new_node;
        q->tail = q->head;
    } else {
        q->tail->next = new_node;
        new_node->prev = q->tail;
        q->tail = new_node;
    }
    atomic_fetch_add(&q->size, 1);
    debug("%d", atomic_load(&q->size));
}

Node deqeue(Queue* q) {
    assert(q != NULL);
    assert(q->head != NULL);
    Node* to_delete = q->head;
    const_T val = to_delete->val;
    q->head = q->head->next;
    if (q->head) q->head->prev = NULL;
    free(to_delete);
    atomic_fetch_sub(&q->size, 1);
    return (Node){val, NULL, NULL};
    // return to_delete;
}

void free_queue(Queue* q) {
    assert(q != NULL);
    Node* walk = q->head;
    while (walk) {
        Node* to_delete = walk;
        walk = walk->next;
        free(to_delete);
    }
}
