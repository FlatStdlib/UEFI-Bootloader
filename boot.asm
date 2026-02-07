extern Init_FSL

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
    sub rsp, 32

    mov rcx, r8
    mov rdx, r9
    call Init_FSL

.hang:
    hlt
    jmp .hang
