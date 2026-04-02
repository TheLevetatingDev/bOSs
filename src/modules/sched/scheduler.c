#include "sched.h"
#include <stddef.h>

// Tell the compiler these are defined in task.c
extern task_t* current_task;
extern task_t* task_list_head;
extern task_t* task_list_tail;

// The assembly function in context_switch.s
extern void switch_context(uint64_t* old_rsp, uint64_t new_rsp);

void sched_init() {
    // Create a dummy task for the main kernel execution
    static task_t kernel_main_task;
    kernel_main_task.id = 0;
    kernel_main_task.state = TASK_RUNNING;
    kernel_main_task.next = NULL;
    current_task = &kernel_main_task;
}

void sched_yield() {
    if (task_list_head == NULL) return; // Nowhere to switch to!

    task_t* old_task = current_task;
    task_t* next_task = current_task->next;

    // Round Robin: If we hit the end of the list, go back to the start
    if (next_task == NULL) {
        next_task = task_list_head;
    }

    current_task = next_task;
    switch_context(&old_task->rsp, next_task->rsp);
}