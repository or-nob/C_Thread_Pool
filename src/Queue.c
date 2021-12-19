#include "Queue.h"

void enqueue(Queue* q, const_T val, T param) {
    assert(q != NULL);
    Node* new_node = (Node*)malloc(sizeof(Node));
    assert(new_node != NULL);
    new_node->val = val;
    new_node->param = param;
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
}

Node deqeue(Queue* q) {
    assert(q != NULL);
    assert(q->head != NULL);
    Node* to_delete = q->head;
    const_T val = to_delete->val;
    T param = to_delete->param;
    q->head = q->head->next;
    if (q->head) q->head->prev = NULL;
    free(to_delete);
    atomic_fetch_sub(&q->size, 1);
    return (Node){val, param, NULL, NULL};
    // return to_delete;
}

void free_queue(Queue* q) {
    Node* walk = q->head;
    while (walk) {
        Node* to_delete = walk;
        walk = walk->next;
        free(to_delete);
    }
}
