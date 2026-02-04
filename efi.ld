ENTRY(efi_main)

SECTIONS {
    . = 0;
    .text : { *(.text*) }
    .data : { *(.data*) }
    .reloc : { *(.reloc) }
}
