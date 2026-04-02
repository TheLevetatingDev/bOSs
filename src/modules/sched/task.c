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
    kprintf("[DEBUG] task_create: Starting...\n");

    if (!entry_point) {
        kprintf("[DEBUG] task_create: NULL entry point!\n");
        return;
    }

    task_t* new_task = (task_t*)kmalloc(sizeof(task_t));
    if (!new_task) {
        kprintf("[DEBUG] task_create: kmalloc failed!\n");
        return;
    }
    kprintf("[DEBUG] task_create: kmalloc success\n");

    void* stack_bottom = pmm_alloc(1);
    if (!stack_bottom) {
        kprintf("[DEBUG] task_create: pmm_alloc failed!\n");
        return;
    }
    kprintf("[DEBUG] task_create: pmm_alloc success\n");

    new_task->stack_bottom = stack_bottom; // Store the stack bottom

    // Stack grows downward from high addresses to low addresses
    // We need to set up a 56-byte context frame at the top of the stack
    // The frame contains: [rbp, rbx, r12, r13, r14, r15, return_address]
    // Based on switch_context.s, when it runs with RSP pointing to rbp location:
    // 1. pop rbp  ; from [RSP]
    // 2. pop rbx  ; from [RSP+8]
    // 3. pop r12  ; from [RSP+16]
    // 4. pop r13  ; from [RSP+24]
    // 5. pop r14  ; from [RSP+32]
    // 6. pop r15  ; from [RSP+40]
    // 7. ret       ; pops return_address from [RSP+48]
    //
    // So if RSP = stack_bottom + 4088:
    // rbp is at 4088, rbx at 4096, r12 at 4104, r13 at 4112, r14 at 4120, r15 at 4128, return_address at 4136
    // But that's beyond the page size!
    //
    // Correct: RSP should point to stack_bottom + 4048 (8 bytes from top)
    // Then: rbp at 4048, rbx at 4056, r12 at 4064, r13 at 4072, r14 at 4080, r15 at 4088, return_address at 4096
    //
    // Frame starts 48 bytes from the top: stack_bottom + (4096 - 48) = stack_bottom + 4048
    uint64_t* stack_frame = (uint64_t*)((uint64_t)stack_bottom + 4048);

    // Set up the context frame that switch_context will pop
    // Indices: rbp is index 0 because it's at the RSP position
    stack_frame[0] = 0; // rbp - will be popped first (from [RSP])
    stack_frame[1] = 0; // rbx - popped from [RSP+8]
    stack_frame[2] = 0; // r12 - popped from [RSP+16]
    stack_frame[3] = 0; // r13 - popped from [RSP+24]
    stack_frame[4] = 0; // r14 - popped from [RSP+32]
    stack_frame[5] = 0; // r15 - popped from [RSP+40]
    stack_frame[6] = (uint64_t)entry_point; // Return address - popped by 'ret' from [RSP+48]

    // RSP must point to where rbp is stored (the first value to be popped)
    new_task->rsp = (uint64_t)stack_bottom + 4048;

    new_task->id = next_id++;
    new_task->state = TASK_READY;
    new_task->next = NULL;
    kprintf("[DEBUG] task_create: Task ID assigned\n");

    // Add to list
    kprintf("[DEBUG] task_create: Adding to list...\n");
    if (task_list_head == NULL) {
        task_list_head = new_task;
        task_list_tail = new_task;
    } else {
        task_list_tail->next = new_task;
        task_list_tail = new_task;
    }
    kprintf("[DEBUG] task_create: Done!\n");
}

void spawn_task(void (*func)()) {
    task_create(func);
    kprintf("[Sched] New task spawned and added to queue.\n");
}
