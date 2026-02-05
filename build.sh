mkdir -p third_party_libs

# x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
#   -I third_party_libs/gnu-efi/inc \
#   -c src/allocator.c -o allocator.obj

# x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
#   -I third_party_libs/gnu-efi/inc \
#   -c src/init.c -o init.obj
  
# x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
#   -I third_party_libs/gnu-efi/inc \
#   -c src/internal.c -o internal.obj

FILES=$(cd src && ls -1 *.c; cd ..)
PWD=$(pwd)
for file in $FILES; do 
  x86_64-w64-mingw32-gcc -ffreestanding -fshort-wchar -mno-red-zone \
    -I third_party_libs/gnu-efi/inc \
    -c "src/$file" -o "${file%.c}.obj"
done
nasm -f win64 boot.asm -o boot.obj


lld-link \
  /subsystem:efi_application \
  /entry:efi_main \
  /nodefaultlib \
  /out:BOOTX64.EFI \
  *.obj

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