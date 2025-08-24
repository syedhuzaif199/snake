#include <stdio.h>

#define QUEUE_IMPLEMENTATION
#include "utils/queue.h"

typedef struct {
    float x, y;
} Vec2;

void print_vec2(Vec2 vec) {
    printf("Vec2 { %.3f, %.3f }\n", vec.x, vec.y);
}

void vec2_queue_data_printer(Queue *q) {
    Vec2 *data = (Vec2 *)q->data;
    printf("Queue Data: [\n");
    for(int i = 0; i < q->capacity; i++) {
        Vec2 vec = data[i];
        char *start = "_";
        char *end = "_";
        if(q->start == i) {
            start = "s";
        }
        if(q->end == i) {
            end = "e";
        }
        printf("%s %s \t%d => ",start, end, i);
        print_vec2(vec);
    }
    printf("]\n");
}


int main() {
    Queue *q = queue_create(sizeof(Vec2), 3);
    print_queue(q, vec2_queue_data_printer);
    enqueue(q, &((Vec2){1,1}));
    vec2_queue_data_printer(q);
    enqueue(q, &((Vec2){2,2}));
    vec2_queue_data_printer(q);
    enqueue(q, &((Vec2){3,3}));
    vec2_queue_data_printer(q);
    enqueue(q, &((Vec2){4,4}));
    vec2_queue_data_printer(q);

    Vec2 *vec = (Vec2 *)dequeue(q);
    if(vec != NULL) {
        printf("Dequeued ");
        print_vec2(*vec);
        vec2_queue_data_printer(q);
    }
    
    vec = (Vec2 *)dequeue(q);
    if(vec != NULL) {
        printf("Dequeued ");
        print_vec2(*vec);
        vec2_queue_data_printer(q);
    }


    vec = (Vec2 *)dequeue(q);
    if(vec != NULL) {
        printf("Dequeued ");
        print_vec2(*vec);
        vec2_queue_data_printer(q);
    }

    enqueue(q, &((Vec2){5,5}));
    vec2_queue_data_printer(q);

    vec = (Vec2 *)dequeue(q);
    if(vec != NULL) {
        printf("Dequeued ");
        print_vec2(*vec);
        vec2_queue_data_printer(q);
    }

    vec = (Vec2 *)dequeue(q);
    if(vec != NULL) {
        printf("Dequeued ");
        print_vec2(*vec);
        vec2_queue_data_printer(q);
    }
}