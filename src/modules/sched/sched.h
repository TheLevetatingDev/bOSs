#ifndef SCHED_H
#define SCHED_H

#include <stdint.h>

typedef enum {
    TASK_READY,
    TASK_RUNNING,
    TASK_SLEEPING,
    TASK_DEAD
} task_state_t;

typedef struct task {
    uint64_t rsp;            // The saved stack pointer
    uint64_t id;             // Process ID
    task_state_t state;      // Current status
    struct task* next;       // Next task in the queue
    void* stack_bottom;      // Base address of the allocated stack
} task_t;

void sched_init();
void sched_yield();
void task_create(void (*entry_point)());

#endif