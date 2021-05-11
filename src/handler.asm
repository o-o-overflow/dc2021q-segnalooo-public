section .text
global _start

_start:
    



    mov r8, qword [rsi] ; sig 5
    mov r9, qword [rsi+8] ; code 1/2/80
    mov r10, qword [rsi+0x10] ; addr
    mov rbp, rsp ; entry rsp, in the last page
    and rbp, 0xfffffffffffff000
    add rbp, 0xff8 ; rbp is bottom of data section
    mov r12, rbp
    mov r12, qword [r12] ; HANDLERP is here

    mov rcx, qword [rbp-0x8]
    inc rcx
    mov qword [rbp-0x8], rcx
    cmp rcx, 4
    jb _less_than_4_sigs
        hlt
    _less_than_4_sigs:

    cmp r8, 0x5
    je _sig_is_5
        hlt

    _sig_is_5:
        cmp r9, 0x80
        jne _code_is_not_80
            ;clean and jump back to r12 + after int3
            mov rbx, r12
            add rbx, 0x50 ; end of code in a.asm
            mov rsp, rbx
            and rsp, 0xfffffffffffff000
            add rsp, 0x1000
            ;clean regs
            xor rax, rax
            ;xor rbx, rbx
            xor rcx, rcx
            xor rdx, rdx
            ;xor rsp, rsp
            xor rbp, rbp
            xor rsi, rsi
            xor rdi, rdi
            xor r8, r8
            xor r9, r9
            xor r10, r10
            xor r11, r11
            xor r12, r12
            xor r13, r13
            xor r14, r14
            xor r15, r15
            ;_loop2:
            ;jmp _loop2
            jmp rbx

        _code_is_not_80:
            ;_loop2:
            ;jmp _loop2


        cmp r9, 2
        jne _code_is_not_2
            ;print len
            mov rbx, r10
            sub rbx, r12
            sub rbx, 0xfc8 ;;; where popf is, this is the 0xf00 offset + 9; BASECODEP + 9
            push rbx
            call _print_reg

            ;_loop3:
            ;jmp _loop3

            ;mprotect rw
            xor rax, rax
            mov al, 10
            mov rdi, r12
            mov rsi, 0x1000
            mov rdx, 3 ; rw
            syscall

            ;write ud2
            mov rcx, r10
            mov byte [rcx], 0x0f 
            inc rcx
            mov byte [rcx], 0x0b

            ;mprotect re
            xor rax, rax
            mov al, 10
            mov rdi, r12
            mov rsi, 0x1000
            mov rdx, 5 ; re
            syscall

            ;_loop3:
            ;jmp _loop3

        _code_is_not_2:
            ;this code should not be executed if code is not 2 (1)

            ;check r10 range between r12 and rcx
            mov rcx, r12
            add rcx, 0x1000

            cmp r12, r10
            jb _e2
            hlt
            _e2:
            cmp rcx, r10
            ja _e3
            hlt
            _e3:

            ;clean and jump back to r10
            mov rbx, r10
            mov rsp, rbx
            and rsp, 0xfffffffffffff000
            add rsp, 0x1000
            ;clean regs
            xor rax, rax
            ;xor rbx, rbx
            xor rcx, rcx
            xor rdx, rdx
            ;xor rsp, rsp
            xor rbp, rbp
            xor rsi, rsi
            xor rdi, rdi
            xor r8, r8
            xor r9, r9
            xor r10, r10
            xor r11, r11
            xor r12, r12
            xor r13, r13
            xor r14, r14
            xor r15, r15
            ;_loop2:
            ;jmp _loop2
            jmp rbx


_loop:
jmp _loop       


    ;push r10
    ;call _print_reg


_print_reg:
    mov rbp, qword [rsp+8]
    add rsp, 8

    _bl:
    mov rbx, rbp
    shr rbx, 60
    cmp rbx, 9
    jle _l2
    add rbx, 0x27
    _l2:
    add rbx, 0x30
    mov qword [rsp], rbx

    mov rdx, 1
    mov rsi, rsp
    mov rdi, 1
    mov rax, 1
    syscall ; write rsp

    shl rbp, 4
    cmp rbp, 0
    jne _bl

    and rbx, 0xf
    cmp rbx, 9
    jg _bl

    mov qword [rsp], 0xa
    mov rdx, 1
    mov rsi, rsp
    mov rdi, 1
    mov rax, 1
    syscall ; write rsp

    sub rsp, 8
    ret

