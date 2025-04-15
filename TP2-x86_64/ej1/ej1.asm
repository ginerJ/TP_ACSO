; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm
global string_proc_node_destroy_asm

extern malloc
extern free
extern str_concat
extern strdup

; typedef struct string_proc_node_t {
;     struct string_proc_node_t* next;       ; offset 0
;     struct string_proc_node_t* previous;   ; offset 8
;     uint8_t type;                          ; offset 16
;     padding                                ; 7 bytes (17–23)
;     char* hash;                            ; offset 24
; } string_proc_node;

; typedef struct string_proc_list_t {
;     string_proc_node* first;               ; offset 0
;     string_proc_node* last;                ; offset 8
; } string_proc_list;

; =======================================================
; string_proc_list_create_asm()
; =======================================================
string_proc_list_create_asm:
    push rbp
    mov rbp, rsp
    mov rdi, 16
    call malloc
    test rax, rax
    jz .return
    mov qword [rax], NULL
    mov qword [rax + 8], NULL
.return:
    pop rbp
    ret

; =======================================================
; string_proc_node_create_asm(uint8_t type, char* hash)
; =======================================================
string_proc_node_create_asm:
    push rbp
    mov rbp, rsp
    sub rsp, 8

    mov rdi, 32
    call malloc
    test rax, rax
    jz .return_null

    mov qword [rax], NULL
    mov qword [rax + 8], NULL
    movzx rcx, dil
    mov byte [rax + 16], cl
    mov rdx, rsi
    mov qword [rax + 24], rdx

.return:
    mov rax, rax
    leave
    ret

.return_null:
    mov rax, NULL
    leave
    ret

; =======================================================
; string_proc_list_add_node_asm(string_proc_list* list, uint8_t type, char* hash)
; =======================================================
string_proc_list_add_node_asm:
    push rbp
    mov rbp, rsp

    mov rbx, rdi               ; guardar list antes de sobrescribir
    movzx rdi, sil             ; type → rdi
    mov rsi, rdx               ; hash → rsi
    call string_proc_node_create_asm
    test rax, rax
    jz .end

    mov rcx, [rbx]
    test rcx, rcx
    jz .empty_list

    mov rcx, [rbx + 8]
    mov [rcx], rax
    mov [rax + 8], rcx
    mov [rbx + 8], rax
    jmp .end

.empty_list:
    mov [rbx], rax
    mov [rbx + 8], rax

.end:
    pop rbp
    ret

; =======================================================
; string_proc_list_concat_asm(string_proc_list* list, uint8_t type, char* hash)
; =======================================================
string_proc_list_concat_asm:
    push rbp
    mov rbp, rsp
    sub rsp, 32

    mov [rbp-8], rdi
    mov [rbp-16], sil
    mov [rbp-24], rdx

    mov rdi, rdx
    call strdup
    mov [rbp-32], rax

    mov rbx, [rbp-8]
    mov rbx, [rbx]
.loop:
    test rbx, rbx
    jz .done
    movzx rcx, byte [rbx + 16]
    cmp cl, [rbp-16]
    jne .next

    mov rdi, [rbp-32]
    mov rsi, [rbx + 24]
    call str_concat

    mov rdi, [rbp-32]
    call free

    mov [rbp-32], rax

.next:
    mov rbx, [rbx]
    jmp .loop

.done:
    mov rax, [rbp-32]
    leave
    ret

; =======================================================
; string_proc_node_destroy_asm(string_proc_node* node)
; =======================================================
string_proc_node_destroy_asm:
    push rbp
    mov rbp, rsp
    test rdi, rdi
    jz .end

    mov qword [rdi], NULL          ; node->next = NULL
    mov qword [rdi + 8], NULL      ; node->previous = NULL
    mov byte [rdi + 16], 0         ; node->type = 0
    ; node->hash no se libera ni modifica

    mov rdi, rdi                   ; pasar ptr a free
    call free

.end:
    pop rbp
    ret
