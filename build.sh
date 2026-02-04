x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
  -I gnu-efi/inc \
  -c test.c -o test.obj

nasm -f win64 boot.asm -o boot.obj

lld-link \
  /subsystem:efi_application \
  /entry:efi_main \
  /nodefaultlib \
  /out:BOOTX64.EFI \
  boot.obj \
  test.obj

rm boot.obj test.obj BOOTX64.lib