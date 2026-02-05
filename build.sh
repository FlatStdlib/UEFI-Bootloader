# x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
#   -I third_party_libs/gnu-efi/inc \
#   -c test.c -o test.obj

mkdir -p third_party_libs

x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
  -I third_party_libs/gnu-efi/inc \
  -c src/allocator.c -o allocator.obj

x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
  -I third_party_libs/gnu-efi/inc \
  -c src/init.c -o init.obj
  
x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
  -I third_party_libs/gnu-efi/inc \
  -c src/internal.c -o internal.obj

nasm -f win64 boot.asm -o boot.obj

# lld-link \
#   /subsystem:efi_application \
#   /entry:efi_main \
#   /nodefaultlib \
#   /out:BOOTX64.EFI \
#   boot.obj


lld-link \
  /subsystem:efi_application \
  /entry:efi_main \
  /nodefaultlib \
  /out:BOOTX64.EFI \
  *.obj

# rm boot.obj test.obj BOOTX64.lib
rm *.obj BOOTX64.lib

download_efi() {
  git clone https://github.com/vathpela/gnu-efi.git
  cd gnu-efi

  make ARCH=x86_64
}

download_edk()
{
  git clone https://github.com/tianocore/edk2.git
  cd edk2
  git submodule update --init
}