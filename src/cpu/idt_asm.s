[bits 64]
global irq0_handler
extern timer_interrupt_handler

irq0_handler:
    push rbp
    mov rbp, rsp

    ; Save all registers
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    ; Align stack to 16 bytes for C code
    mov r12, rsp
    and rsp, -16

    call timer_interrupt_handler

    ; Restore stack and registers
    mov rsp, r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
    pop rbp
    iretq