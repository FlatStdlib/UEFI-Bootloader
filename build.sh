mkdir -p third_party_libs

FSL_EFI_FILES=$(cd src && ls -1 *.c; cd ..)
EFI_LIBC_FILES=$(cd src/libc && ls -1 *.c && cd ..)
EFI_FS_FILES=$(cd src/fs && ls -1 *.c && cd ..)
PWD=$(pwd)

# Compile to Object File(s) #
for file in $FSL_EFI_FILES; do 
  x86_64-w64-mingw32-gcc \
  -ffreestanding \
  -fshort-wchar \
  -mno-red-zone \
  -fno-builtin \
  -fno-stack-protector \
  -mno-stack-arg-probe \
  -lgnuefi -lefi \
  -I third_party_libs/gnu-efi/inc \
  -c "src/$file" -o "${file%.c}.obj"
done

nasm -f win64 boot.asm -o boot.obj


### WINDOWS LINKER ###
lld-link \
  /subsystem:efi_application \
  /entry:efi_main \
  /nodefaultlib \
  /out:BOOTX64.EFI \
  *.obj

rm *.obj # BOOTX64.lib

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