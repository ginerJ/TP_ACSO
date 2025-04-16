%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

extern malloc
extern free
extern str_concat
extern strlen
extern strcpy
extern strcat

string_proc_list_create_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 16
        mov     edi, 16
        call    malloc
        mov     qword [rbp-8], rax
        cmp     qword [rbp-8], 0
        jne     .L2
        mov     eax, 0
        jmp     .L3
.L2:
        mov     rax, qword [rbp-8]
        mov     qword [rax], 0
        mov     rax, qword [rbp-8]
        mov     qword [rax+8], 0
        mov     rax, qword [rbp-8]
.L3:
        leave
        ret
string_proc_node_create_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 32
        mov     eax, edi
        mov     qword [rbp-32], rsi
        mov     byte [rbp-20], al
        mov     edi, 32
        call    malloc
        mov     qword [rbp-8], rax
        cmp     qword [rbp-8], 0
        jne     .L5
        mov     eax, 0
        jmp     .L6
.L5:
        mov     rax, qword [rbp-8]
        mov     qword [rax], 0
        mov     rax, qword [rbp-8]
        mov     qword [rax+8], 0
        mov     rax, qword [rbp-8]
        movzx   edx, byte [rbp-20]
        mov     byte [rax+16], dl
        mov     rax, qword [rbp-8]
        mov     rdx, qword [rbp-32]
        mov     qword [rax+24], rdx
        mov     rax, qword [rbp-8]
.L6:
        leave
        ret
string_proc_list_add_node_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 48
        mov     qword [rbp-24], rdi
        mov     eax, esi
        mov     qword [rbp-40], rdx
        mov     byte [rbp-28], al
        movzx   eax, byte [rbp-28]
        mov     rdx, qword [rbp-40]
        mov     rsi, rdx
        mov     edi, eax
        call    string_proc_node_create_asm
        mov     qword [rbp-8], rax
        cmp     qword [rbp-8], 0
        je      .L12
        mov     rax, qword [rbp-24]
        mov     rax, qword [rax]
        test    rax, rax
        jne     .L10
        mov     rax, qword [rbp-24]
        mov     rdx, qword [rbp-8]
        mov     qword [rax], rdx
        jmp     .L11
.L10:
        mov     rax, qword [rbp-24]
        mov     rax, qword [rax+8]
        mov     rdx, qword [rbp-8]
        mov     qword [rax], rdx
        mov     rax, qword [rbp-24]
        mov     rdx, qword [rax+8]
        mov     rax, qword [rbp-8]
        mov     qword [rax+8], rdx
.L11:
        mov     rax, qword [rbp-24]
        mov     rdx, qword [rbp-8]
        mov     qword [rax+8], rdx
        jmp     .L7
.L12:
        nop
.L7:
        leave
        ret
string_proc_list_concat_asm:
        push    rbp
        mov     rbp, rsp
        sub     rsp, 64
        mov     qword [rbp-40], rdi
        mov     eax, esi
        mov     qword [rbp-56], rdx
        mov     byte [rbp-44], al
        cmp     qword [rbp-40], 0
        je      .L14
        cmp     qword [rbp-56], 0
        jne     .L15
.L14:
        mov     eax, 0
        jmp     .L16
.L15:
        mov     rax, qword [rbp-56]
        mov     rdi, rax
        call    strlen
        add     rax, 1
        mov     rdi, rax
        call    malloc
        mov     qword [rbp-8], rax
        cmp     qword [rbp-8], 0
        jne     .L17
        mov     eax, 0
        jmp     .L16
.L17:
        mov     rdx, qword [rbp-56]
        mov     rax, qword [rbp-8]
        mov     rsi, rdx
        mov     rdi, rax
        call    strcpy
        mov     rax, qword [rbp-40]
        mov     rax, qword [rax]
        mov     qword [rbp-16], rax
        jmp     .L18
.L20:
        mov     rax, qword [rbp-16]
        movzx   eax, byte [rax+16]
        cmp     byte [rbp-44], al
        jne     .L19
        mov     rax, qword [rbp-16]
        mov     rdx, qword [rax+24]
        mov     rax, qword [rbp-8]
        mov     rsi, rdx
        mov     rdi, qword [rbp-8]
        call    str_concat
        mov     qword [rbp-24], rax
        mov     rax, qword [rbp-8]
        mov     rdi, rax
        call    free
        mov     rax, qword [rbp-24]
        mov     qword [rbp-8], rax
.L19:
        mov     rax, qword [rbp-16]
        mov     rax, qword [rax]
        mov     qword [rbp-16], rax
.L18:
        cmp     qword [rbp-16], 0
        jne     .L20
        mov     rax, qword [rbp-8]
.L16:
        leave
        ret

