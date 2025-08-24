#ifndef QUEUE_H
#define QUEUE_H
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef struct {
    void *data;
    size_t capacity;
    size_t data_size;
    int start;
    int end;
} Queue;
Queue *queue_create(size_t data_size, size_t capacity);
bool enqueue(Queue* queue, void *data);
void *peek_start(Queue *queue);
void *peek_end(Queue *queue);
void *dequeue(Queue *queue);
void queue_free(Queue *queue);
typedef void (*queue_data_printer)(Queue *queue);
void print_queue(Queue *q, queue_data_printer data_printer);
#endif // QUEUE_H


#ifdef QUEUE_IMPLEMENTATION

Queue *queue_create(size_t data_size, size_t capacity) {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if(!q) {
        #ifdef QUEUE_H_WARNINGS
        fprintf(stderr, "Error: failed to create queue\n");
        #endif
    }
    q->data = malloc(data_size * capacity);
    if(!q->data) {
        #ifdef QUEUE_H_WARNINGS
        fprintf(stderr, "Error: failed to allocate buffer for queue\n");
        #endif
    }
    q->data_size = data_size;
    q->capacity = capacity;
    q->start = -1;
    q->end = 0;
    return q;
}

bool enqueue(Queue* queue, void *data) {
    if(queue->end == queue->start) {
        #ifdef QUEUE_H_WARNINGS
        fprintf(
            stderr,
            "Warning: queue capacity of %zu exceeded! "
            "Make sure that this case is handled by checking if enqueue() returns false.\n",
            queue->capacity
        );
        #endif
        return false;
    }
    if(queue->start == -1) {
        if(queue->end != 0) {
        #ifdef QUEUE_H_WARNINGS
            fprintf(
                stderr,
                "Error: queue->end != 0 when queue->start == -1:"
                "when the queue is empty, queue->start == -1 and queue->end == 0\n"
            );
        #endif
        }
        queue->start = 0;
    }
    char *bytes = (char *)queue->data;
    memcpy(&bytes[queue->end * queue->data_size], data, queue->data_size);
    queue->end += 1;
    queue->end %= queue->capacity;
}
void *dequeue(Queue *queue) {
    if(queue->start == -1) {
        #ifdef QUEUE_H_WARNINGS
        fprintf(stderr, "Warning: trying to dequeue from an empty queue! NULL pointer returned! Make sure the case is handled.\n");
        #endif
        return NULL;
    }
    void *ret = (void *)((uint8_t *)queue->data + queue->start * queue->data_size);
    queue->start += 1;
    queue->start %= queue->capacity;
    if(queue->start == queue->end) {
        queue->start = -1;
        queue->end = 0;
    }
    return ret;
}

void *peek_start(Queue *queue) {
    if(queue->start == -1) {
        return NULL;
    }
    return (void *)((uint8_t *)queue->data + queue->start * queue->data_size);
}

void *peek_end(Queue *queue) {
    if(queue->start == -1) {
        return NULL;
    }

    return (void *)((uint8_t *)queue->data + queue->end * queue->data_size);
}

void queue_free(Queue *queue) {
    free(queue->data);
    free(queue);
}

void print_queue(Queue *q, queue_data_printer data_printer) {
    printf("Queue: {\n");
    printf("\tcapacity: %zu\n", q->capacity);
    printf("\tdata_size: %zu\n", q->data_size);
    printf("\tstart: %d\n", q->start);
    printf("\tend: %d\n", q->end);
    printf("}\n");

    data_printer(q);
}
#endif // QUEUE_IMPLEMENTATION