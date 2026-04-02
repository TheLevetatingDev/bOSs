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
    if (!new_task) return;

    void* stack_bottom = pmm_alloc(1); 
    if (!stack_bottom) return;

    // IF pmm_alloc RETURNS A PHYSICAL ADDRESS, UNCOMMENT THIS:
    // extern uint64_t hhdm_offset; // (Or however you get Limine's HHDM)
    // stack_bottom = (void*)((uint64_t)stack_bottom + hhdm_offset);

    // Treat the stack as an array of 64-bit integers
    uint64_t* stack = (uint64_t*)((uint64_t)stack_bottom + 4096);

    // --- PART A: THE IRETQ FRAME ---
    stack[-1] = 0x10;                  // SS (Data Segment)
    stack[-2] = (uint64_t)stack;       // RSP (Point to the exact top of the stack, 4096)
    stack[-3] = 0x202;                 // RFLAGS (Interrupts enabled)
    stack[-4] = 0x08;                  // CS (Code Segment)
    stack[-5] = (uint64_t)entry_point; // RIP (Function entry)

    // --- PART B: THE ISR STUB VALUES (Crucial!) ---
    // If your assembly interrupt exits with `add rsp, 16` before `iretq`, 
    // you MUST include these two dummy values.
    stack[-6] = 0;                     // Dummy Error Code
    stack[-7] = 0;                     // Dummy Interrupt Number
    
    // --- PART C: THE GENERAL REGISTERS ---
    // 15 registers pushed by your assembly context switch
    int offset = 7; 
    for (int i = 0; i < 15; i++) {
        offset++;
        stack[-offset] = 0;
    }

    // Save the final stack pointer. 
    // '&stack[-offset]' safely gets the address of the last item pushed.
    new_task->rsp = (uint64_t)&stack[-offset];
    
    new_task->id = next_id++;
    new_task->state = TASK_READY;
    new_task->next = NULL;

    // Add to list
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