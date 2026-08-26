import os, sys, subprocess

dependencies = [
	"sudo apt install gcc-mingw-w64-x86-64 -y",
	"sudo apt install lld -y",
	"sudo apt install xorriso -y",
	"git clone https://git.code.sf.net/p/gnu-efi/code gnu-efi",
	"cd gnu-efi",
	"make",
	"sudo make install"
]

if "--dep" in sys.argv:
	for cmd in dependencies: subprocess.getoutput(cmd)
	exit(0)

ALL_FILES = []
SOURCE_CODE_FILES = [
    "ls -1 src/*.c",
    "ls -1 src/libc/*.c",
    "ls -1 src/fs/*.c",
    "ls -1 src/tools/*.c",
    "ls -1 src/libgfb/*.c",
]

if "--clear" in sys.argv:
    print(subprocess.getoutput("clear"), end="\r")

""" GET C FILES """
for dir in SOURCE_CODE_FILES:
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
subprocess.getoutput("nasm -f win64 boot/boot.asm -o boot.obj")

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

iso_cmds = [
    "cp /usr/share/OVMF/OVMF_VARS_4M.fd ./OVMF_VARS.fd",
    "rm -rf iso esp.img fsl-os.iso",
    "mkdir -p iso/EFI/BOOT",
    "cp build/BOOTX64.EFI iso/EFI/BOOT/BOOTX64.EFI",
    "dd if=/dev/zero of=esp.img bs=1M count=64",
    "mkfs.fat -F 32 esp.img",
    "sudo mkdir -p /mnt/esp",
    "sudo mount -o loop esp.img /mnt/esp",
    "sudo mkdir -p /mnt/esp/EFI/BOOT",
    "sudo cp build/BOOTX64.EFI /mnt/esp/EFI/BOOT/BOOTX64.EFI",
    "sudo umount /mnt/esp",
    "cp esp.img iso/esp.img",
    "xorriso -as mkisofs -R -J -V \"FSL-OS\" -e /esp.img -no-emul-boot -o fsl-os.iso iso/",
    "rm -rf esp.img iso"
]

if "--iso" in sys.argv:
    for cmd in iso_cmds:
        subprocess.run(cmd, shell = True)

# if "--2usb" in sys.argv:

print("\x1b[32mISO Successfully Created!\x1b[39m")
