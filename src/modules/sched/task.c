#include "sched.h"
#include "../../mm/pmm.h"
#include "../../mm/kmalloc.h"
#include <stddef.h>

// Definitions of the globals
task_t* current_task = NULL;
task_t* task_list_head = NULL;
task_t* task_list_tail = NULL;
static uint64_t next_id = 1;
extern void kprintf(const char *str);

void task_create(void (*entry_point)()) {
    task_t* new_task = (task_t*)kmalloc(sizeof(task_t)); 
    void* stack = pmm_alloc(1); 
    uint64_t* stack_ptr = (uint64_t*)((uint64_t)stack + 4096);

    // --- INITIAL STACK SETUP ---
    *(--stack_ptr) = 0x10;                    // SS
    
    // Fix: Calculate the RSP value BEFORE decrementing again
    uint64_t rsp_val = (uint64_t)stack_ptr; 
    *(--stack_ptr) = rsp_val;                 // RSP
    
    *(--stack_ptr) = 0x202;                   // RFLAGS
    *(--stack_ptr) = 0x08;                    // CS
    *(--stack_ptr) = (uint64_t)entry_point;   // RIP

    for(int i = 0; i < 6; i++) {
        *(--stack_ptr) = 0; // RBP, RBX, R12-R15
    }

    new_task->rsp = (uint64_t)stack_ptr;
    new_task->id = next_id++;
    new_task->state = TASK_READY;
    new_task->next = NULL;

    if (task_list_head == NULL) {
        task_list_head = new_task;
        task_list_tail = new_task;
    } else {
        task_list_tail->next = new_task;
        task_list_tail = new_task;
    }
}

// Add this to task.c
void spawn_task(void (*func)()) {
    task_create(func);
    kprintf("[Sched] New task spawned and added to queue.\n");
}