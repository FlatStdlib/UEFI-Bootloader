extern my_c_function

BITS 64
DEFAULT REL

section .text
global efi_main

efi_main:
    ; Microsoft x64 ABI:
    ; RCX = ImageHandle
    ; RDX = EFI_SYSTEM_TABLE*

    push rbp
    mov rbp, rsp
    sub rsp, 32            ; shadow space (MANDATORY)

    mov r8, rdx         ; save SystemTable* temporarily

    ; print message
    mov rcx, [r8 + 0x60] ; ConOut
    mov rdx, message
    mov rax, [rcx + 0x18] ; OutputString
    call rax

    ; call C function
    mov rcx, r8          ; restore SystemTable* for C function
    call my_c_function

.hang:
    hlt
    jmp .hang

section .data
message:
    dw 'U','E','F','I',' ','B','O','O','T',' ','O','K',13,10,0
