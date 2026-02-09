import os, subprocess

ALL_FILES = []

""" GET C FILES """
for dir in ["ls -1 src/*.c", "ls -1 src/libc/*.c", "ls -1 src/fs/*.c", "ls -1 src/tools/*.c"]:
    FILES = subprocess.getoutput(dir).strip().split("\n")
    for file in FILES: ALL_FILES.append(file)

print("Compiling sources.....")
""" Compile all C files """
for file in ALL_FILES:
    file_name = file.split("/")[-1].replace(".c", ".obj")
    print(f"[ + ] {file} -> {file_name}")
    chk = subprocess.getoutput(f"""x86_64-w64-mingw32-gcc \
        -ffreestanding \
        -fshort-wchar \
        -mno-red-zone \
        -fno-builtin \
        -fno-stack-protector \
        -mno-stack-arg-probe \
        -lgnuefi -lefi \
        -I third_party_libs/gnu-efi/inc \
        -c {file} -o {file_name}"""
    )
    
    if chk:
        print(chk)

""" Compile the boot loader """
subprocess.getoutput("nasm -f win64 boot.asm -o boot.obj")

chk = subprocess.getoutput("""lld-link \
    /subsystem:efi_application \
    /entry:efi_main \
    /nodefaultlib \
    /out:build/BOOTX64.EFI \
    *.obj"""
)

if chk:
    print(chk)

subprocess.getoutput("rm *.obj build/BOOTX64.lib")

"""
xorriso -as mkisofs \
  -iso-level 3 \
  -o output.iso \
  -full-iso9660-filenames \
  -volid "MY_EFI_ISO" \
  -eltorito-alt-boot \
  -e EFI/BOOT/BOOTX64.EFI \
  -no-emul-boot \
  -isohybrid-gpt-basdat \
  ISO_ROOT/
"""