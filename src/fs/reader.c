#include "../init.h"

/* THIS IS SUPPOSEDLY PROVIDED TO YOU BUT W.E */
EFI_GUID gEfiBlockIoProtocolGuid = {
    0x964e5b21, 0x6459, 0x11d2,
    {0x8e, 0x39, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x3b}
};

public EFI_BLOCK_IO_PROTOCOL *usb_find_raw_block(void)
{
    EFI_STATUS st;
    EFI_HANDLE *handles = NULL;
    UINTN count = 0;

    println(L"[+] Enumerating handles for Block IO...");
    st = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiBlockIoProtocolGuid,
        NULL,
        &count,
        &handles
    );

    if(EFI_ERROR(st) || count == 0) {
        println(L"[-] No handles found");
        return NULL;
    }

    for(UINTN i = 0; i < count; i++) {
        EFI_BLOCK_IO_PROTOCOL *blk = NULL;

        st = gBS->HandleProtocol(
            handles[i],
            &gEfiBlockIoProtocolGuid,
            (VOID **)&blk
        );

        if(EFI_ERROR(st) || !blk || !blk->Media)
            continue;

        if(blk->Media->MediaPresent && blk->Media->RemovableMedia && !blk->Media->LogicalPartition) {

            println(L"[+] Found removable Block IO device");
            return blk;
        }
    }

    println(L"[-] No removable Block IO device found");
    return NULL;
}

public EFI_STATUS usb_read_lba(EFI_BLOCK_IO_PROTOCOL *blk, UINT64 lba, UINTN blocks, VOID **out_buf)
{
    if(!blk || !out_buf || blocks == 0)
        return EFI_INVALID_PARAMETER;

    UINTN size = blocks * blk->Media->BlockSize;

    EFI_STATUS st = gBS->AllocatePool(EfiLoaderData, size, out_buf);
    if(EFI_ERROR(st))
        return st;

    st = blk->ReadBlocks(
        blk,
        blk->Media->MediaId,
        lba,
        size,
        *out_buf
    );

    if(EFI_ERROR(st)) {
        gBS->FreePool(*out_buf);
        *out_buf = NULL;
    }

    return st;
}

public fn hex_dump(const UINT8 *buf, UINTN size)
{
    for(UINTN i = 0, nl = 0; i < size; i++) {
        u16 BUFF[3];
        byte_to_hex(buf[i], BUFF);
        print(BUFF);
        print(L", ");
        if (i == nl + 30) {
            i += 30;
            println(NULL);
        }
    }

    if(size % 16)
        println(NULL);
}
