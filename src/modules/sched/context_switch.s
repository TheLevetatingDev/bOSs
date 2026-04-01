[bits 64]
global switch_context

switch_context:
    ; Save current task state
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov [rdi], rsp    ; old_task->rsp = rsp
    mov rsp, rsi      ; rsp = new_task->rsp

    ; Restore new task state
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp

    ret